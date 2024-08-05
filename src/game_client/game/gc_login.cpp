#include "gc_include.h"
#include "gc_cursor.h"
#include "gc_login.h"
#include "region_client.h"
#include "gc_login_session.h"
#include "gs_protocol.h"
#include "lgs_gws.h"
#include "ui_form_msg.h"
#include "ui_form_textMsg.h"
#include <net/g_tcpsession.h>
#include <unordered_map>
#include "../preConsole.h"
#include <graph/rtg_matrix.h>
#include <string>

namespace {
enum CreateChatIds { ZhanShi = 1, FengWu = 4, ShuShi = 7, DaoShi = 10 };

struct CharSkinInfo {
    std::string act;
    std::string body;
    std::string hand;
    std::string leg;
};

static std::unordered_map<int, CharSkinInfo> s_mCharSkinInfo = {
    {ZhanShi, {"pf", "bpf011", "hpf011", "lpf011"}},
    {FengWu, {"ph", "bph001", "hph001", "lph001"}},
    {ShuShi, {"pn", "bpn011", "hpn011", "lpn011"}},
    {DaoShi, {"pt", "bpt001", "hpt001", "lpt001"}}};

// �������н�ɫ������ӳ��

static int s_CreatActIds_FromCsv[] = {ZhanShi, FengWu, ShuShi, DaoShi};

char* s_pszLoginCharName[] = {"pf", "ph", "pn", "pt"};
char* s_pszCharCreateBody[] = {"bpf011", "bph001", "bpn011", "bpt001"};
char* s_pszCharCreateHand[] = {"hpf011", "hph001", "hpn011", "hpt001"};
char* s_pszCharCreateLeg[] = {"lpf011", "lph001", "lpn011", "lpt001"};

//    std::rotate(arr.begin(), arr.begin() + index, arr.end());
//����ģ�͹��������� 4x3 ����RtgMatrix12��,����������� 4x4 ����RtgMatrix16������Ҫһ��ת������
inline static void Lyy_AdjustCameraMatrix(RtgMatrix16* matOut, RtgMatrix12* matIn) {
    RTASSERT(matOut);
    RTASSERT(matIn);
    RtgVertex3 x = *(RtgVertex3*)&matIn->_00;
    RtgVertex3 y = *(RtgVertex3*)&matIn->_10;
    RtgVertex3 z = *(RtgVertex3*)&matIn->_20;
    x.Normalize();
    y.Normalize();
    z.Normalize();
    matOut->Identity();
    matOut->SetRow(0, x);
    matOut->SetRow(1, y);
    matOut->SetRow(2, z);
    matOut->SetRow(3, (float*)&matIn->_30);
}
}  // namespace

std::string GcLogin::m_szAccountUsername = "";  //���⾲̬������ʼ��
std::string GcLogin::m_szAccountPassword = "";

GcLogin::GcLogin(CGameClientFrame* pGameClientFrame) : m_ini(true) {
    m_pGameClientFrame = pGameClientFrame;
}

GcLogin::~GcLogin() {}

void GcLogin::LoginErrMsg(EErrMsg eMsg, const char* szRetStr, short sRetCode) {
    const char* pMsg;
    switch (eMsg) {
        case GLEM_DISCONNECT:
            pMsg = R(LMSG_DISCONNECT);
            break;  //��¼�����ж�
        case GLEM_CANNOT_CONNECT:
            pMsg = R(LMSG_CANNOT_CONNECT);
            break;  //������������
        case GLEM_CONNECT_FAIL:
            pMsg = R(LMSG_CONNECT_FAIL);
            break;  //���ӵ�¼������ʧ��
        case GLEM_LOGIN_FAIL_NOT_FOUND:
            pMsg = R(LMSG_LOGIN_FAIL_NOT_FOUND);
            break;  //�û���������
        case GLEM_LOGIN_FAIL_PWD_WRONG:
            pMsg = R(LMSG_LOGIN_FAIL_PWD_WRONG);
            break;  //��¼ʧ��,���������������֤���Ƿ���ȷ
        case GLEM_LOGIN_FAIL:
            pMsg = R(LMSG_LOGIN_FAIL);
            break;  //��¼ʧ��
        case GLEM_LOGIN_FAIL_USER_ONLINE:
            pMsg = R(LMSG_LOGIN_FAIL_USER_ONLINE);
            break;  //�ʺ����ڱ�ʹ��
        case GLEM_LOGIN_FAIL_ACCOUNT_STOP:
            pMsg = R(LMSG_LOGIN_FAIL_ACCOUNT_STOP);
            break;  //�ʺű���ͣ
        case GLEM_LOGIN_FAIL_ACCOUNT_PAUSE:
            pMsg = R(LMSG_LOGIN_FAIL_ACCOUNT_PAUSE);
            break;  //�������
        case GLEM_LOGIN_FAIL_ACCOUNT_WRONG5:
            pMsg = R(LMSG_LOGIN_FAIL_ACCOUNT_WRONG5);
            break;  //�������������ʺ��������Ϊ�����û���Ϣ��ȫ�������ʺ�10����
        case GLEM_LOGIN_FAIL_POT_NOT_ENOUGH:
            pMsg = R(LMSG_LOGIN_FAIL_POT_NOT_ENOUGH);
            break;  //�ʺŵ�������
        case GLEM_GET_ACCOUNT_FAIL:
            pMsg = R(LMSG_GET_ACCOUNT_FAIL);
            break;  //��ȡ�ʺ���Ϣʧ��
        case GLEM_CREATE_RESULT_EXIST:
            pMsg = R(LMSG_CREATE_RESULT_EXIST);
            break;  //��ɫ�Ѿ�����
        case GLEM_CREATE_RESULT_FAIL:
            pMsg = R(LMSG_CREATE_RESULT_FAIL);
            break;  //������ɫʧ��
        case GLEM_CREATE_FACTION_ERROR:
            pMsg = R(LMSG_CREATE_RESULT_FACTION_ERROR);
            break;  //������ɫʧ��,ֻ�ܴ���ͬһ��Ӫ�Ľ�ɫ
        case GLEM_CREATE_RESULT_INVALID_NAME:
            pMsg = R(LMSG_CREATE_RESULT_INVALID_NAME);
            break;  //������ɫʧ��,��ɫ�����Ϊ�˸��ַ�
        case GLEM_CREATE_BANNED_NAME:
            pMsg = R(LMSG_CREATE_CHAR_NAME_BANNED);
            break;  //��ɫ���ư���Υ���ַ�
        case GLEM_RESTORE_RESULT_FAIL:
            pMsg = R(LMSG_RESTORE_RESULT_FAIL);
            break;  //�ָ���ɫʧ��
        case GLEM_DELETE_RESULT_FAIL:
            pMsg = R(LMSG_DELETE_RESULT_FAIL);
            break;  //ɾ����ɫʧ��
        case GLEM_LOGIN_FAIL_SERVER_FULL:
            pMsg = R(LMSG_LOGIN_FAIL_SERVER_FULL);
            break;  //����������,���Ժ�����
        case GLEM_LOGIN_FAIL_SERVER_STOP:
            pMsg = R(LMSG_LOGIN_FAIL_SERVER_STOP);
            break;  //������ά����
        case GLEM_LOGIN_FAIL_NEW_CARD:
            pMsg = R(LMSG_LOGIN_FAIL_NEW_CARD);
            PostQuitMessage(22);
            break;
        case GLEM_LOGIN_FAIL_PUBWIN_CENTER:
            pMsg = R(LMSG_LOGIN_FAIL_PUBWIN_CENTER);
            break;  //ͬ����Ա����ʧ��,���Ժ�����
        case GLEM_LOGIN_FAIL_IS_LOCKED:
            pMsg = R(LMSG_LOGIN_FAIL_IS_LOCKED);
            break;  //�ʺű�����
        case GLEM_NONE:
        default:
            pMsg = R(LMSG_LOGIN_FAIL_UNKNOWE);
            break;
    }
    if (szRetStr) {
        rt2_sprintf(g_strStaticBuffer, "%s [%s]", pMsg, szRetStr);
        ShowMessage(g_strStaticBuffer);
    } else if (sRetCode) {
        if (sRetCode == 110) {  //lyymark ǿ�Ƶ�¼����
            UIFormMsg* pFrm =
                UIFormMsg::ShowStatic(pMsg, UIFormMsg::TYPE_OK_CANCEL, false, "forcelogin_");
            pFrm->EvOK = RTW_CALLBACK_1(g_layerLogin, UILayerLogin, OnClicked_ForceLogin, pFrm);
            pFrm->EvCancel =
                RTW_CALLBACK_1(g_layerLogin, UILayerLogin, OnClicked_ForceLoginCancel, pFrm);
        } else {
            rt2_sprintf(g_strStaticBuffer, "%s", pMsg);
            ShowMessage(g_strStaticBuffer);
        }
    } else {
        ShowMessage(pMsg);
    }
}

//lyymark 2.GcLogin.LoadLoginSection ����login.ini�����ļ�
void GcLogin::LoadLoginSection(RtIni* pIni, const std::string& szSectionName,
                               std::unordered_map<std::string, CRT_ActorInstance*>& mapActor) {
    guard;
    RtString           szLink, szName;
    CRT_ActorInstance *pBody = nullptr, *pActor = nullptr;
    // ����INI�ļ���ָ�����ƵĽ�
    pIni->FindSection(szSectionName.c_str());
    // ��ȡ��һ����Ŀ
    if (pIni->FirstEntry(&szLink, &szName)) {
        do {
            pActor = RtcGetActorManager()->CreateActor(szName, true);  // ����һ��actorʵ��
            if (!pActor)
                continue;
            // �������CreateChar���򲥷Ŷ��� CreateChar���浥������
            if (szSectionName != "CreateChar") {
                pActor->PlayPose(true);
            }
            if (szLink == "yangguang") {
                pActor->PlayPose("scene", true);
            }
            mapActor[std::string(szLink)] = pActor;  // ��actorʵ����ӵ�ӳ����
            if (szLink == "Body") {  // �����������Ϊ"Body"���򽫵�ǰactorʵ����Ϊ����
                pBody = pActor;
            } else {
                if (!pBody)
                    continue;
                pActor->LinkParent(pBody, szLink.c_str());  // ����pBody
            }

        } while (pIni->NextEntry(&szLink, &szName));
    }
    unguard;
}

bool GcLogin::InitOnce() {
    guard;
    return true;
    unguard;
}

bool GcLogin::ClearOnce() {
    for (auto it = m_mapLogin.begin(); it != m_mapLogin.end();) {
        RtcGetActorManager()->ReleaseActor(it->second);
        it = m_mapLogin.erase(it);
    }
    for (auto it = m_mapSelectChar.begin(); it != m_mapSelectChar.end();) {
        RtcGetActorManager()->ReleaseActor(it->second);
        it = m_mapSelectChar.erase(it);
    }
    for (auto it = m_mapCreateChar.begin(); it != m_mapCreateChar.end();) {
        RtcGetActorManager()->ReleaseActor(it->second);
        it = m_mapCreateChar.erase(it);
    }
    for (auto it = m_selectChar_IDMapGcActor.begin(); it != m_selectChar_IDMapGcActor.end();) {
        DEL_ONE(it->second);
        it = m_selectChar_IDMapGcActor.erase(it);
    }
    RtcGetActorManager()->ReleaseActor(m_pZsDao);
    RtcGetActorManager()->ReleaseActor(m_pZsDao2);
    RtcGetActorManager()->ReleaseActor(m_pMmGong);
    RtcGetActorManager()->ReleaseActor(m_pSsJian);
    RtcGetActorManager()->ReleaseActor(m_pSsJianPath);
    RtcGetActorManager()->ReleaseActor(m_pDsLun);
    RtcGetActorManager()->ReleaseActor(m_pDsLunPath);
    GetDevice()->m_dwClearColor = 0x00000000;
    UIFormTextMsg::Clear();
    UIFormMsg::Clear(false);
    return true;
}

void GcLogin::ConfigureForState(const std::unordered_map<std::string, CRT_ActorInstance*>& actorMap,
                                const std::string& graphConfig) {
    this->m_mapActor = actorMap;
    Lyy_UpdateCameraPos();
    UpdateGraphConfig(graphConfig.c_str());
}

bool GcLogin::LeaveCurrentState(EStatus state) {
    switch (state) {
        case GLS_LOADING:
            LeaveLoading();
            return true;
        case GLS_SELECT_GAMEWORLD_SERVER:
            return LeaveSelectGameWorldServer();
        case GLS_LOGIN:
            return LeaveLogin();
        case GLS_SELECT_CHAR:
            return LeaveSelectChar();
        case GLS_CREATE_CHAR:
            return LeaveCreateChar();
        default:
            return true;
    }
}

void GcLogin::EnterNewState(EStatus state) {
    switch (state) {
        case GLS_LOADING:
            EnterLoading();
            break;
        case GLS_SELECT_GAMEWORLD_SERVER:
            ConfigureForState(m_mapLogin, "Graph_Login");
            EnterSelectGameWorldServer();
            break;
        case GLS_LOGIN:
            ConfigureForState(m_mapLogin, "Graph_Login");
            EnterLogin();
            break;
        case GLS_SELECT_CHAR:
            ConfigureForState(m_mapSelectChar, "Graph_SelectChar");
            EnterSelectChar();
            break;
        case GLS_CREATE_CHAR:
            ConfigureForState(m_mapCreateChar, "Graph_CreateChar");
            EnterCreateChar();
            break;
    }
}

//lyymark 2.GcLogin ��¼״̬��
void GcLogin::SetLoginState(EStatus eState) {
    guard;
    // �뿪��һ��״̬
    if (!LeaveCurrentState(m_eCurrentStatus))
        return;
    // ����״̬
    m_eCurrentStatus = eState;
    // ������״̬
    EnterNewState(m_eCurrentStatus);
    unguard;
}

CRT_ActorInstance* GcLogin::FindModel(const std::string& szName) {
    guard;
    auto it = m_mapActor.find(szName);
    if (it != m_mapActor.end())
        return it->second;
    return nullptr;
    unguard;
}

// lyymark 2.GcLogin �޸����λ�þ��� by lyy 2024-07-30
void GcLogin::Lyy_UpdateCameraPos() {
    m_pBody = FindModel("Body");
    if (!m_pBody) {
        return;
    }
    RtgMatrix12 _mat;
    if (m_pBody->GetBoneMat(&_mat, "bcam") || m_pBody->GetBoneMat(&_mat, "Bcam")) {
        RtgMatrix16 mCamera;
        Lyy_AdjustCameraMatrix(&mCamera, &_mat);
        GetDevice()->m_pCamera->SetMatrix(mCamera);
    }
}

void GcLogin::EnterSelectGameWorldServer() {
    DXUtil_Timer(TIMER_RESET);  //���ڽ��֡�����ƿ��ټ���
    if (g_layerLogin == nullptr) {
        UILayer::EnterLogin();
    }
    StartGetGameWorldServer();             //��ʼ��ȡ��Ϸ���������
    g_layerLogin->mp_layerServer->Show();  //��ʾ�������б�
}

bool GcLogin::LeaveSelectGameWorldServer() {
    CHECK(m_eCurrentStatus == GLS_SELECT_GAMEWORLD_SERVER);
    g_layerLogin->mp_layerServer->Hide();
    EndGetGameWorldServer();  //������ȡ��Ϸ���������
    return true;
}

void GcLogin::ReadAccountFromFile() {
    // ��ȡ�û���
    RtIni       iniUser;
    const char* szIniUsername = "";
    const char* szIniPassword = "";
    if (iniUser.OpenFile(R(INI_USER))) {
        const char* szSave = iniUser.GetEntry("account", "save");
        bool        isSave = (szSave && atol(szSave) > 0);
        g_layerLogin->mp_ckSaveAcc->SetChecked(isSave);
        if (isSave) {
            szIniUsername = iniUser.GetEntry("login", "username");
            szIniPassword = iniUser.GetEntry("login", "password");
        }
        iniUser.CloseFile();
    }
    // ��ֵ�û���
    m_szAccountUsername = szIniUsername ? szIniUsername : "";
    m_szAccountPassword = szIniPassword ? szIniPassword : "";

    g_layerLogin->mp_txtAccout->Enable();
    g_layerLogin->mp_txtPwd->Enable();
    // ���ÿؼ�ֵ
    g_layerLogin->mp_txtAccout->SetText(m_szAccountUsername);
    g_layerLogin->mp_txtPwd->SetText(m_szAccountPassword);

    // �������뽹��
    if (m_szAccountUsername.empty()) {
        g_layerLogin->mp_txtAccout->SetFocus();
    } else {
        g_layerLogin->mp_txtPwd->SetFocus();
    }
}

void GcLogin::EnterLogin() {
    if (g_layerLogin == nullptr) {
        UILayer::EnterLogin();
    }
    ReadAccountFromFile();  //��ȡ������˺�
    g_layerLogin->Show();
}

bool GcLogin::LeaveLogin() {
    guard;
    CHECK(m_eCurrentStatus == GLS_LOGIN);
    g_layerLogin->Hide();
    return true;
    unguard;
}

// edit by lyy  2024.8.5
void GcLogin::EnterSelectChar() {
    guard;
    DXUtil_Timer(TIMER_RESET);  //���ڽ��֡�����ƿ��ټ���
    if (g_layerSelectChar == nullptr) {
        UILayer::EnterSelectChar();
    }
    LOAD_UI("Pnbutton")->Show();
    LOAD_UI("btnenter")->Show();
    LOAD_UI("btndeletechar")->Show();
    LOAD_UI("btnsetcharpwd")->Show();
    LOAD_UI("btnsetcharpwd")->Disable();  //��ֹ���ý�ɫ���� ��Ϊ����д��
    LOAD_UI("btnRMB")->Show();
    LOAD_UI("btnRMB")->Disable();  //��ֹ��ֵ���� ��Ϊ����д��
    LOAD_UI("RoleInfo")->Hide();
    m_bCanInput = true;
    //m_curSelCharIndex = -1;//����Ҫ����ѡ��״̬����Ȼÿ�ζ�Ҫ����ѡ��
    UpdateSelectChar();
    unguard;
}

void GcLogin::UpdateSelectChar() {
    guard;
    // ��鵱ǰ״̬�Ƿ�Ϊѡ���ɫ״̬�������򷵻�
    if (m_eCurrentStatus != GLS_SELECT_CHAR)
        return;
    m_selectChar_IDMapIndex.clear();
    //��ȡ�˺���Ϣ
    const GcAccountInfo& accountInfo = GetAccountInfo();

    if (accountInfo.chatCount == 0 || m_curSelCharIndex == -1) {  //�����ɫ����Ϊ0��û��ѡ�н�ɫ
        LOAD_UI("btnenter")->Disable();                           //��ֹ������Ϸ
        LOAD_UI("btndeletechar")->Disable();                      //��ֹɾ����ɫ
    }
    if (accountInfo.chatCount < 3)
        LOAD_UI("btncreaterole")->Enable();
    else
        LOAD_UI("btncreaterole")->Disable();

    // ��ȡ��ť������ȫ����ʼ��
    for (auto& btn : g_layerSelectChar->m_charBtnArray) {
        btn->Disable();
        btn->SetText("��");
        btn->SetTextColor(0xffffffff);  //��ɫ
    }
    // �ٸ��ݽ�ɫ����������Ӧ�İ�ť�����ΪMaxUserCharBtn=3
    for (std::size_t i = 0; i < accountInfo.chatCount && i < UILayerSelectChar::MaxUserCharBtn;
         i++) {
        g_layerSelectChar->m_charBtnArray[i]->Enable();
    }

    GcActor*          pGcActor = nullptr;
    const SUserActor* pUserActorInCsv = nullptr;
    const SCreModel*  pActorModelInCsv = nullptr;
    // �����û���ɫ�б�,������Ӧģ�ͣ��Ѿ������Ļ�����
    for (std::size_t i = 0; i < accountInfo.chatCount; i++) {
        auto& userInfo = accountInfo.users[i];
        if (m_selectChar_IDMapGcActor.find(userInfo.id) != m_selectChar_IDMapGcActor.end())
            continue;
        // �����û���Ϣ�������ɫactorID
        // �� user_actor.csv����ҽ�ɫ��Ϣ,������Ӧģ��id��
        pUserActorInCsv = g_TableUserActor.FindUserActor(userInfo.attributes.actorID);
        if (!pUserActorInCsv)
            continue;
        pActorModelInCsv = g_TableCreModel.FindCreModel(pUserActorInCsv->ModelId);
        if (!pActorModelInCsv)
            continue;
        // �����½�ɫ����
        pGcActor = RT_NEW GcActor;
        pGcActor->CreateGraphData((SCreModel*)pActorModelInCsv,
                                  userInfo.headModelID);  //�ϳɽ�ɫģ��
        pGcActor->SetName(userInfo.name);                 //�û����ݿ���еĽ�ɫ��
        pGcActor->DisplayActor(true);
        // ���ؽ�ɫ��HUD��ͷ����ʾ��Ϣ��
        if (pGcActor->m_pHUD) {
            pGcActor->m_pHUD->Hide();
        }
        // ���ý�ɫ��ְҵ���Ա�����
        pGcActor->m_core.Metier = userInfo.attributes.metier;
        pGcActor->m_core.Sex = userInfo.attributes.Sex;
        // ���ؽ�ɫ����Ʒ��Ϣ
        CItemManager* pItemMgr = ((CGameClientFrame*)GetApp())->m_pItemManager;
        if (pItemMgr) {
            SItemID item;
            if (pGcActor->mItem.LoadFromString(userInfo.items) <= 0) {
                // �����Ʒ��Ϣ��Ч�����ص�¼״̬����ʾ������Ϣ
                SetLoginState(GLS_LOGIN);
                ShowMessage(R(LMSG_USERDATA_INVALID));
                return;
            }
        }
        if (pGcActor)
            m_selectChar_IDMapGcActor[userInfo.id] = pGcActor;
    }
    //���¿�ѡ��ɫ�б�
    for (std::size_t index = 0; index < accountInfo.chatCount; index++) {
        const auto& userInfo = accountInfo.users[index];
        if (m_selectChar_IDMapGcActor.find(userInfo.id) != m_selectChar_IDMapGcActor.end()) {
            m_selectChar_IDMapIndex[userInfo.id] = index;
        }
    }
    if (!m_pBody)
        return;
    RtgMatrix12 _SlotMatrix;
    size_t      maxCount = std::min(accountInfo.chatCount, (long)UILayerSelectChar::MaxUserCharBtn);
    //ʵ����ֻ��ɾ������ӽ�ɫ����Ҫ���¸���λ�ã���������д�� by lyy 2024.8.5
    for (std::size_t i = 0; i < maxCount; ++i) {
        auto&       user = accountInfo.users[i];
        std::string boneName = "bno" + std::to_string(i + 1);
        auto*       gcActor = m_selectChar_IDMapGcActor[user.id];
        gcActor->mBaseActor.GetGraph()->p()->LinkParent(m_pBody, boneName.c_str());
        std::string roleName = user.name;
        SUserActor* pActor = g_TableUserActor.FindUserActor(user.attributes.metier,        //ְҵ
                                                            user.attributes.metierLevel);  //��ת
        if (!pActor)
            P_LOGERROR("user_actor���ȡ����ְҵΪ��" + user.attributes.metier);
        //���ö�Ӧ�����ϵĽ�ɫ����
        g_layerSelectChar->m_charBtnArray[i]->SetText(roleName);
        //����ǵ�ǰѡ��Ľ�ɫ
        if (i == m_curSelCharIndex) {
            g_layerSelectChar->m_charBtnArray[m_curSelCharIndex]->SetTextColor(0xffff0000);  //��ɫ
            LOAD_UI("btnenter")->Enable();
            LOAD_UI("btndeletechar")->Enable();
            std::string Lev = "Lv: " + std::to_string(user.attributes.level);
            std::string Faction = g_layerSelectChar->m_faction[user.attributes.faction];
            std::string Miter = pActor->MetierName;
            LOAD_UI("RoleInfo")->Show();
            LOAD_UI("RoleInfo.RoleName")->SetText(roleName);
            LOAD_UI("RoleInfo.Lev")->SetText(Lev);
            LOAD_UI("RoleInfo.Faction")->SetText(Faction);
            LOAD_UI("RoleInfo.Miter")->SetText(Miter);
            P_LOGINFO("��ǰѡ��Ľ�ɫidΪ:" + std::to_string(user.id) +
                      "������Ϊ :" + std::to_string(m_curSelCharIndex));
        }
    }
    unguard;
}

bool GcLogin::LeaveSelectChar() {
    guard;
    CHECK(m_eCurrentStatus == GLS_SELECT_CHAR);
    m_bCanInput = false;
    LOAD_UI("Pnbutton")->Hide();
    LOAD_UI("btnenter")->Hide();
    LOAD_UI("btndeletechar")->Hide();
    LOAD_UI("btnsetcharpwd")->Hide();
    LOAD_UI("btnRMB")->Hide();
    LOAD_UI("RoleInfo")->Hide();
    //������ת��ť
    //LOAD_UI("btnuserleft")->Hide();
    //LOAD_UI("btnuserright")->Hide();
    m_selectUserCharIds.clear();
    return true;
    unguard;
}

void GcLogin::SetSelectUser(int iSel) {
    if (iSel == m_iCurSelectChar) {
        return;
    }
    switch (m_eCurrentStatus) {
        case GLS_SELECT_CHAR: {
            m_iCurSelectChar = iSel;
            UpdateSelectChar();
        } break;
        case GLS_CREATE_CHAR: {
            m_iCurSelectChar = iSel;
            vector<SHeadModel> heads;
            if (!g_TableHeadModel.GetHeadModel(s_CreatActIds_FromCsv[m_iCurSelectChar], heads))
                return;

            //m_creatActorList[m_iCurSelectChar]->UnloadSkin(heads[m_ePrevHeadID].skin.c_str());
            //m_creatActorList[m_iCurSelectChar]->LoadSkin(heads[0].skin.c_str(), true);
            LOAD_UI("fmcreatid2.fmhair.lbhairname")->SetText(heads[0].name);
            LOAD_UI("fmcreatid2.fmface.lbfacename")->SetText("�ݲ�����");
            m_ePrevHeadID = m_eNextHeadID = 0;
        } break;
        default:
            break;
    }
}

// ѡ����Ӫ add by lyy 2024.8.3
void GcLogin::SetSelectShangOrZhou(int iSei) {
    //����
    if (iSei == None) {
        m_selectFaction = None;
        if (auto door = FindModel("Door")) {
            door->PlayPose("close");
            door->RegisterNotify(nullptr);
        }
        LOAD_UI("fmorder")->Show();
        LOAD_UI("fmcreatid")->Hide();
        return;
    }

    if (iSei != Shang && iSei != Zhou)
        return;

    switch (iSei) {
        case Shang: {
            auto* t_body = FindModel("Body");
            auto* Tray_Shang = FindModel("Tray_Shang");
            auto* Tray_Zhou = FindModel("Tray_Zhou");
            auto* BG_Shang = FindModel("BG_Shang");
            auto* BG_Zhou = FindModel("BG_Zhou");
            Tray_Zhou->SetVisible(0.f);
            BG_Zhou->SetVisible(0.f);
            Tray_Shang->SetVisible(1.f);
            BG_Shang->SetVisible(1.f);
            if (Tray_Shang && BG_Shang && t_body) {
                Tray_Shang->LinkParent(t_body, "btray");
                BG_Shang->LinkParent(t_body, "bbg");
            }

            break;
        }
        case Zhou: {
            auto* t_body = FindModel("Body");
            auto* Tray_Shang = FindModel("Tray_Shang");
            auto* BG_Shang = FindModel("BG_Shang");
            auto* Tray_Zhou = FindModel("Tray_Zhou");
            auto* BG_Zhou = FindModel("BG_Zhou");
            Tray_Shang->SetVisible(0.f);
            BG_Shang->SetVisible(0.f);
            Tray_Zhou->SetVisible(1.f);
            BG_Zhou->SetVisible(1.f);
            if (Tray_Zhou && BG_Zhou && t_body) {
                Tray_Zhou->LinkParent(t_body, "btray");
                BG_Zhou->LinkParent(t_body, "bbg");
            }
            break;
        }
        default:
            // ������������������Ҫ��
            break;
    }
    //����

    if (auto door = FindModel("Door")) {
        door->PlayPose("open");
        door->RegisterNotify(nullptr);
    }
    m_selectFaction = iSei;
    LOAD_UI("fmorder")->Hide();
    LOAD_UI("fmcreatid")->Show();
}

bool GcLogin::LoadModel(const std::string& modelName, CRT_ActorInstance** model,
                        std::string linkName /*= ""*/, CRT_ActorInstance** parent /*= nullptr*/) {
    if (*model == nullptr) {
        *model = RtcGetActorManager()->CreateActor(modelName.c_str(), true);
        if (*model == nullptr) {
            P_LOGERROR("[GcActor::Render] �޷���������ģ��:" + modelName);
            return false;
        }
        if (!linkName.empty() && *parent != nullptr) {
            (*model)->LinkParent(*parent, linkName.c_str());
        }
    }
    return true;
}

//lyymark 3.Gclogin �����ɫ����
void GcLogin::EnterCreateChar() {
    guard;
    if (!m_pBody)
        return;
    m_crtChar_csvIdMapIndex.clear();
    //��ʼ����
    if (auto door = FindModel("Door")) {
        door->RealUseFrame(13);  //��ʼ���ŵ�״̬Ϊ��13֡  �ر�״̬
        door->RegisterNotify(nullptr);
    }
    m_iCurSelectChar = -1;
    m_curCrtCharIndex = -1;
    // ���ɿ�ѡ������б�
    CRT_ActorInstance* pActor;
    const SUserActor*  p_userActCsv;
    RtgMatrix12        WeaponMatrix;
    RtgMatrix16        WorldMatrix;
    //��� m_crtChar_csvIdMapActIns Ϊ�գ���ζ��û�д����κν�ɫģ��

    if (m_crtChar_csvIdMapActIns.size() == 0) {
        for (const auto& csvID : s_CreatActIds_FromCsv) {
            const auto& usrActData = s_mCharSkinInfo.at(csvID);
            p_userActCsv = g_TableUserActor.FindUserActor(csvID);
            if (!p_userActCsv || (!p_userActCsv->bZCreate && !p_userActCsv->bSCreate))
                continue;
            if (m_crtChar_csvIdMapActIns.find(csvID) != m_crtChar_csvIdMapActIns.end())
                continue;
            pActor = FindModel(usrActData.act);
            if (!pActor)
                continue;
            pActor->LoadSkin(usrActData.body.c_str());
            pActor->LoadSkin(usrActData.hand.c_str());
            pActor->LoadSkin(usrActData.leg.c_str());
            if (g_TableHeadModel.GetHeadModel(csvID, m_crtChar_heads[csvID])) {
                pActor->LoadSkin(m_crtChar_heads[csvID][0].skin.c_str(), true);
            }
            switch (csvID) {
                case ZhanShi:
                    LoadModel("iw0401dao_04", &m_pZsDao, "Box01", &pActor);
                    LoadModel("iw0401dao_04", &m_pZsDao2, "Box02", &pActor);
                    pActor->LinkParent(m_pBody, "bno1");
                    break;
                case FengWu:
                    LoadModel("iw0401gong_04", &m_pMmGong, "Box01", &pActor);
                    pActor->LinkParent(m_pBody, "bno2");
                    break;
                case ShuShi:
                    pActor->LinkParent(m_pBody, "bno3");
                 /*   LoadModel("ss_fj", &m_pSsJianPath);
                    LoadModel("iw0501jian_04", &m_pSsJian, "Blink0", &m_pSsJianPath);
                    pActor->LinkParent(m_pBody, "bno3");
                    WeaponMatrix = WorldMatrix;
                    WeaponMatrix._32 += 15.f;
                    m_pSsJianPath->SetMatrix(WeaponMatrix);
                    m_pSsJianPath->LinkParent(pActor);*/
                    m_pSsJianPath = RtcGetActorManager()->CreateActor("ss_fj", true);
                    m_pSsJian = RtcGetActorManager()->CreateActor("iw0501jian_04", true);
                    m_pSsJian->LinkParent(m_pSsJianPath, "Blink0");
                    m_pSsJianPath->LinkParent(pActor);
                    WeaponMatrix = WorldMatrix;
                    m_pSsJianPath->SetMatrix(WeaponMatrix);
                    break;
                case DaoShi:
                    LoadModel("iw0401huan_04", &m_pDsLun);
                    LoadModel("ss_fl", &m_pDsLunPath);
                    pActor->LinkParent(m_pBody, "root");
                    WeaponMatrix = WorldMatrix;
                    m_pDsLunPath->SetMatrix(WeaponMatrix);
                    m_pDsLun->LinkParent(m_pDsLunPath, "Blink0");
                    m_pDsLunPath->LinkParent(pActor);
                    break;
            }
            m_crtChar_csvIdMapActIns[csvID] = pActor;
        }
    }
    int index = 0;
    for (auto& it : m_crtChar_csvIdMapActIns) {
        m_crtChar_csvIdMapIndex[index++] = it.first;
    }
    OnEnterCreateChar();
    unguard;
}

bool GcLogin::LeaveCreateChar() {
    guard;
    CRT_ActorInstance* pActor;
    if (m_bCanInput) {
        CHECK(m_eCurrentStatus == GLS_CREATE_CHAR);
        if (m_iCurSelectChar < 0 || m_iCurSelectChar > 4) {
            pActor = NULL;
        }
        m_iCurSelectChar = -1;
        OnLeaveCreateChar();
    }
    m_creatActorList.clear();
    return true;
    unguard;
}

void GcLogin::OnEnterCreateChar() {
    if (g_layerSelectChar == nullptr) {
        UILayer::EnterSelectChar();
    }
    m_bCanInput = true;
    LOAD_UI("fmorder")->Show();
    LOAD_UI("fmcreatid")->Hide();

    // Modified By Wayne Wong 2010-11-26 for demo
    LOAD_UI("fmmingwang")->Show();
    LOAD_UI("fmshengwu")->Show();
    LOAD_UI("lbmiaojiangtext")->Show();
    LOAD_UI("lbwutaitext")->Show();
    LOAD_UI("fmhuajian")->Show();
    LOAD_UI("fmshushan")->Show();
    LOAD_UI("lbshushantext")->Show();
    LOAD_UI("lbhuajiantext")->Show();
}

void GcLogin::OnLeaveCreateChar() {
    m_bCanInput = false;
    LOAD_UI("fmorder")->Hide();
    LOAD_UI("fmcreatid")->Hide();
    LOAD_UI("fmcreatid1")->Hide();
    LOAD_UI("fmcreatid2")->Hide();
    LOAD_UI("fmcreatid3")->Hide();
    LOAD_UI("fmmingwang")->Hide();
    LOAD_UI("fmhuajian")->Hide();
    LOAD_UI("fmshushan")->Hide();
    LOAD_UI("fmshengwu")->Hide();
    LOAD_UI("btnmale")->Hide();
    LOAD_UI("btnfemale")->Hide();
    LOAD_UI("lbshushantext")->Hide();
    LOAD_UI("lbhuajiantext")->Hide();
    LOAD_UI("lbmiaojiangtext")->Hide();
    LOAD_UI("lbwutaitext")->Hide();
}

//loadingҳ�����
void GcLogin::EnterLoading() {
    guard;
    UILayer::EnterLoading();
    m_bLoading = false;
    unguard;
}

bool GcLogin::LeaveLoading() {
    guard;
    // �򿪱�������
    const char* szMusicFileName = GetGameIni()->GetEntry("Audio", "LoginMusic");
    if (szMusicFileName) {
        g_pBackMusic->Play(szMusicFileName, true);
    }
    UILayer::LeaveLoading();

    return true;
    unguard;
}

//renderloding
void GcLogin::OnLoading() {
    guard;
    const char* szLoginConfigName = "scene/login.ini";

    if (!m_ini.OpenFile(szLoginConfigName)) {
        ERR1("����: ���ܵ�¼���������ļ�[%s].\n", szLoginConfigName);
        return;
    }
    m_ini.CloseFile();

    LoadLoginSection(&m_ini, "Login", m_mapLogin);
    LoadLoginSection(&m_ini, "SelectChar", m_mapSelectChar);
    LoadLoginSection(&m_ini, "CreateChar", m_mapCreateChar);

    // �򿪵�½����
    UILayer::EnterLogin();
    unguard;
}

void GcLogin::UpdateGraphConfig(const char* szName) {
    // ��������ӳ��Ƕȣ�Field of View������λΪ��
    float fCameraFOV = 45.0f;
    // ��Ⱦ�ӿڵĿ�߱ȣ�����4:3
    float fCameraAspect = 4.f / 3.f;
    //������
    float fCameraNear = 10.0f;
    //Զ����
    float fCameraFar = 30000.0f;
    //��Ч����ʼ�ľ���
    float fFogNear = 1000.f;
    //��Ч�������ľ���
    float fFogFar = 8000.f;
    //��Ч������
    long lFogEnable = 0;
    //��չ����ɫ����������Ϊ���ɫ
    long lSkyLight = 0xFF888888;
    //����Դ�ĺ�ɫ����ǿ��
    float fLightR = 70.f;
    //����Դ�Ŀ�ȣ�0��ʾ���Դ
    float fLightW = 0.f;
    //������Դ�ĺ�ɫ����ǿ��
    float fLight2R = 70.f;
    //������Դ�Ŀ�ȣ�0��ʾ���Դ
    float fLight2W = 0.f;
    m_lSkyFog = false;
    RtgVertex3 vSkyLight(0.5f, 0.5f, 0.5f);
    RtgVertex3 vFogLight(0.5f, 0.5f, 0.5f);
    char*      pBuf;

    m_ini.GetEntry(szName, "CameraFOV", &fCameraFOV);
    m_ini.GetEntry(szName, "CameraAspect", &fCameraAspect);
    m_ini.GetEntry(szName, "CameraNear", &fCameraNear);
    m_ini.GetEntry(szName, "CameraFar", &fCameraFar);
    m_ini.GetEntry(szName, "FogNear", &fFogNear);
    m_ini.GetEntry(szName, "FogFar", &fFogFar);
    m_ini.GetEntry(szName, "FogEnable", &lFogEnable);
    m_ini.GetEntry(szName, "SkyFog", &m_lSkyFog);
    m_ini.GetEntry(szName, "LightR", &fLightR);
    m_ini.GetEntry(szName, "LightW", &fLightW);
    m_ini.GetEntry(szName, "Light2R", &fLight2R);
    m_ini.GetEntry(szName, "Light2W", &fLight2W);
    pBuf = (char*)m_ini.GetEntry(szName, "SkyLight");
    if (pBuf) {
        sscanf(pBuf, "%f,%f,%f", &(vSkyLight.x), &(vSkyLight.y), &(vSkyLight.z));
        vSkyLight /= 255.f;
    }
    pBuf = (char*)m_ini.GetEntry(szName, "SunLight");
    if (pBuf) {
        sscanf(pBuf, "%f,%f,%f", &(m_lightDirect.vDiffuse.x), &(m_lightDirect.vDiffuse.y),
               &(m_lightDirect.vDiffuse.z));
        m_lightDirect.vDiffuse /= 255.f;
        m_lightDirect.vSpecular = m_lightDirect.vDiffuse;
    }
    pBuf = (char*)m_ini.GetEntry(szName, "SunLight2");
    if (pBuf) {
        sscanf(pBuf, "%f,%f,%f", &(m_lightDirect2.vDiffuse.x), &(m_lightDirect2.vDiffuse.y),
               &(m_lightDirect2.vDiffuse.z));
        m_lightDirect2.vDiffuse /= 255.f;
        m_lightDirect2.vSpecular = m_lightDirect2.vDiffuse;
        m_bLight2 = true;
    } else {
        m_bLight2 = false;
    }
    pBuf = (char*)m_ini.GetEntry(szName, "FogColor");
    if (pBuf) {
        sscanf(pBuf, "%f,%f,%f", &(vFogLight.x), &(vFogLight.y), &(vFogLight.z));
        vFogLight /= 255.f;
    }

    GetDevice()->m_pCamera->SetProjParams(
        DegreeToRadian(fCameraFOV), GetDevice()->m_pCamera->m_fAspect, fCameraNear, fCameraFar);
    GetDevice()->SetFogParameter(RtgVectorToColor(vFogLight, 1.f), fFogNear, fFogFar);
    GetDevice()->SetFogEnable(lFogEnable);
    GetDevice()->SetRenderState(RTGRS_ENVIRONMENT_LIGHTING, RtgVectorToColor(vSkyLight, 1.f));
    RtgMatrix16 m16;
    m16.Unit();
    m16.RotateLZ(DegreeToRadian(fLightW));
    m16.RotateLX(DegreeToRadian(fLightR + 180.f));
    m16.GetRow(2, m_lightDirect.vDirection);
    m_lightDirect.vDirection.Normalize();
    m_lightDirect.eMode = RTG_LIGHT_DIRECTION;

    m16.Unit();
    m16.RotateLZ(DegreeToRadian(fLight2W));
    m16.RotateLX(DegreeToRadian(fLight2R + 180.f));
    m16.GetRow(2, m_lightDirect2.vDirection);
    m_lightDirect2.vDirection.Normalize();
    m_lightDirect2.eMode = RTG_LIGHT_DIRECTION;
}

//�����ɫ����
void GcLogin::OnLeftRotation() {
    // m_selectUserCharActs[m_iCurSelectChar]->GetGraph()->p()->RotateLZ(DegreeToRadian(30.f));
}

static int mRotationTime = 0;

void GcLogin::OnLeftRotationDown() {
    mRotationTime++;
    // m_selectUserCharActs[m_iCurSelectChar]->GetGraph()->p()->RotateLZ((mRotationTime * 30) % 360);
}

void GcLogin::OnLeftRotationUp() {}

//�����ɫ����
void GcLogin::OnRightRotation() {
    //m_selectUserCharActs[m_iCurSelectChar]->GetGraph()->p()->RotateLZ(DegreeToRadian(-30.f));
}

//������Ϸ
void GcLogin::OnSelectUserDone() {
    guard;
    if (m_curSelCharIndex >= 0 && m_curSelCharIndex < UILayerSelectChar::MaxUserCharBtn) {
        UILayer::EnterLoading();
        SelectChar(GetAccountInfo().users[m_curSelCharIndex].id);
    } else {
        ShowMessage(R(LMSG_PLS_CHOOSE_CHAR));
    }
    unguard;
}

void GcLogin::OnSelectUserWithPwd() {
    UILayer::EnterLoading();
    SelectChar(GetAccountInfo().users[m_curSelCharIndex].id);
}

void GcLogin::OnRestoreChar() {
    guard;
    if (m_curSelCharIndex >= 0 && m_curSelCharIndex < UILayerSelectChar::MaxUserCharBtn) {
        RestoreChar(GetAccountInfo().users[m_curSelCharIndex].id);
    } else {
        ShowMessage(R(LMSG_PLS_CHOOSE_CHAR));
    }
    unguard;
}

void GcLogin::OnRestoreCharSuccess() {
    UIFormMsg::ShowStatic(R(MSG_CHARACTER_RECOVEREDSUCCESS));
    QueryAccountInfo();
}

void GcLogin::OnDeleteUser() {
    guard;
    if (m_curSelCharIndex >= 0 && m_curSelCharIndex < UILayerSelectChar::MaxUserCharBtn) {
        DeleteChar(GetAccountInfo().users[m_curSelCharIndex].id);
    } else {
        ShowMessage(R(LMSG_PLS_CHOOSE_CHAR));
    }
    unguard;
}

//���ý�ɫ����
void GcLogin::OnSetCharPassword(const char* password) {
    guard;
    if (m_curSelCharIndex >= 0 && m_curSelCharIndex < UILayerSelectChar::MaxUserCharBtn) {
        SetCharPassword(GetAccountInfo().users[m_curSelCharIndex].id, password);
    }
    unguard;
}

//�����ɫ����
void GcLogin::OnChangeCharPassword(const char* old, const char* newpassword) {
    guard;
    if (m_curSelCharIndex >= 0 && m_curSelCharIndex < UILayerSelectChar::MaxUserCharBtn) {
        ChangeCharPassword(GetAccountInfo().users[m_curSelCharIndex].id, old, newpassword);
    }
    unguard;
}

//���������
void GcLogin::OnDelCharPassword(const char* password) {
    guard;
    if (m_curSelCharIndex >= 0 && m_curSelCharIndex < UILayerSelectChar::MaxUserCharBtn) {
        DelCharPassword(GetAccountInfo().users[m_curSelCharIndex].id, password);
    }
    unguard;
}

void GcLogin::OnCharPasswordConfirm(const char* password) {
    guard;
    if (m_curSelCharIndex >= 0 && m_curSelCharIndex < UILayerSelectChar::MaxUserCharBtn) {
        CharPasswordConfirm(GetAccountInfo().users[m_curSelCharIndex].id, password);
    }
    unguard;
}

void GcLogin::OnNetDeleteUser(long id, char hasDel) {
    if (hasDel) {
        // ����ָ���� id ��Ӧ��Ԫ��
        auto it = m_selectChar_IDMapGcActor.find(id);
        // ����ҵ������Ԫ��
        if (it != m_selectChar_IDMapGcActor.end()) {
            // ɾ�������Դ
            DEL_ONE(it->second);
            // �� map ��ɾ����Ԫ��
            m_selectChar_IDMapGcActor.erase(it);
        }
    }
    //���»�ȡ�û���Ϣ
    QueryAccountInfo();
}

void GcLogin::OnRandomCreateUser() {}

void GcLogin::OnCreateUser() {
    guard;
    CHECK(m_eCurrentStatus == GLS_CREATE_CHAR);

    if (m_iCurSelectChar == -1) {
        ShowMessage(R(LMSG_PLS_CHOOSE_CHAR));
    }
    if (LOAD_UI("fmcreatid1.fmname.txtname")->GetText().length() < 4 ||
        LOAD_UI("fmcreatid1.fmname.txtname")->GetText().length() > 14) {
        ShowMessage(R(MSG_USERNAME_INVALID));
        return;
    }

    if (!LOAD_UI_T(CUiCheckButton, "btnmale")->GetChecked() &&
        !LOAD_UI_T(CUiCheckButton, "btnfemale")->GetChecked()) {
        ShowMessage(R(MSG_CHARACTER_SELECTSEXUAL));
        return;
    }
    std::string sUsername = LOAD_UI("fmcreatid1.fmname.txtname")->GetText().c_str();
    const char* szUsername = sUsername.c_str();
    if (m_iCurSelectChar < 0)
        return;
    char cActorID = (char)m_selectUserCharIds[m_iCurSelectChar];

    int iCharIndex = GetCharIndexByActorID(m_selectUserCharIds[m_iCurSelectChar]);

    vector<SHeadModel> heads;
    if (g_TableHeadModel.GetHeadModel(s_CreatActIds_FromCsv[iCharIndex], heads)) {
        if (m_HeadModelIndex[iCharIndex] < (int)heads.size()) {
            if (bRandom) {
                headRandomModelID = heads[m_ePrevHeadID].id;
            } else {
                headModelID = heads[m_HeadModelIndex[iCharIndex]].id;
            }
        }
    }
    if (bRandom) {
        this->CreateChar(szUsername, cActorID, FACTION_NEUTRAL, m_bSex, headRandomModelID,
                         m_strCharPassword.c_str(), iRandomAnimalIndex, headRandomImageID);
    } else {
        this->CreateChar(szUsername, cActorID, FACTION_NEUTRAL, m_bSex, headModelID,
                         m_strCharPassword.c_str(), iAnimalIndex, headImageID);
    }
    unguard;
}

//lyymark 2.GcLogin.OnRun GcLogin֡��ѭ�����
void GcLogin::OnRun(float fSecond) {
    //lyytodo ��֡ ��Ϊ����graph��ѭ���������ͳһ�趨
    //CLockFrame lockFrame(1000.0 / 60);
    static RtgVertex3 highColor(1.0, 1.0, 1.0);
    static RtgVertex3 normalColor(0.0, 0.0, 0.0);
    int               i = 0, j = 0;
    POINT             cursorPt;
    static float      fDiffSecond = 0.f;
    fDiffSecond += fSecond;

    const RtgMatrix12* oldWorldMatrix;
    RtgMatrix12        newWorldMatrix;
    //lyymark 2.GcLogin ��������
    g_pSoundMgr->UpdateAll(NULL, GetDevice()->GetAppTime());
    //���³������
    for (auto& pair : m_mapActor) {
        auto& act = pair.second;
        if (act) {
            act->Tick(fSecond * 1000.f, false);
        }
    }
    //�ж�����
    if (m_eCurrentStatus == GLS_SELECT_GAMEWORLD_SERVER) {
        OnGuideNetProcess(fSecond);  // �򵼵�����
    } else {
        this->Process();  // ��½������
    }
    //���³������ﶯ��
    switch (m_eCurrentStatus) {
        case GcLogin::GLS_SELECT_CHAR:
            for (auto& [ID, GcActor] : m_selectChar_IDMapGcActor) {
                if (!GcActor)
                    continue;
                if (!GcActor->GetGraph()->p()->IsPlayingPose()) {
                    GcActor->mBaseActor.PlayPose(GcBaseActor::POSE_STAND);
                }
                GcActor->Run(fSecond);
            }
            break;
        case GcLogin::GLS_CREATE_CHAR:
            //���·ɽ�����
            if (m_pSsJian) {
                m_pSsJian->Tick(fSecond * 1000.f, false);
            }
            if (m_pDsLun) {
                m_pDsLun->Tick(fSecond * 1000.f, false);
            }
            if (!m_pSsJianPath->IsPlayingPose()) {
                m_pSsJianPath->PlayPose("wait_j0", false, 0.5f);
            }
            if (!m_pDsLunPath->IsPlayingPose()) {
                m_pDsLunPath->PlayPose("waiting_l1", false, 0.5f);
            }
            for (auto& [csvID, actInstance] : m_crtChar_csvIdMapActIns) {
                if (!actInstance)
                    continue;
                oldWorldMatrix = actInstance->GetWorldMatrix();
                newWorldMatrix.SetRow(3, *oldWorldMatrix->Row(3));
                actInstance->SetWorldMatrix(newWorldMatrix);
                if (!actInstance->IsPlayingPose()) {
                    if (csvID == ZhanShi) {
                        actInstance->PlayPose("wait_t_sin", false);
                    } else {
                        actInstance->PlayPose("wait_login");
                    }
                }
            }
            break;
    }
    if (m_eCurrentStatus == GLS_SELECT_CHAR || m_eCurrentStatus == GLS_CREATE_CHAR) {
        if (fDiffSecond > 0.1f) {
            fDiffSecond = 0.0f;
            if (!g_workspace.IsMouseMonopolist() && m_bCanInput) {
                GetCursorPos(&cursorPt);
                ScreenToClient((HWND)GetApp()->GetWndHandle(), &cursorPt);
                if (cursorPt.x > 0 && cursorPt.x < GetDevice()->m_iWndWidth && cursorPt.y > 0 &&
                    cursorPt.y < GetDevice()->m_iWndHeight && GetDevice()->HasFocus()) {
                    if (m_creatActorList.size()) {
                        for (i = 0; i < m_creatActorList.size(); i++) {
                            if (m_creatActorList[i] == NULL)
                                continue;
                            if (DetectIntersection(cursorPt.x, cursorPt.y, m_creatActorList[i]))
                                break;
                        }

                        for (j = 0; j < m_creatActorList.size(); j++) {
                            if (m_creatActorList[j] == NULL)
                                continue;
                            if (i != j) {
                                m_creatActorList[j]->SetBaseColor(normalColor, true);
                            } else {
                                m_creatActorList[j]->SetBaseColor(highColor, true);
                            }
                        }
                        if (m_iCurSelectChar >= 0 && m_iCurSelectChar != i) {
                            if (m_creatActorList.size() > m_iCurSelectChar &&
                                m_creatActorList[m_iCurSelectChar]) {
                                m_creatActorList[m_iCurSelectChar]->SetBaseColor(highColor, true);
                            }
                        }
                    }
                    if (m_selectChar_IDMapGcActor.size()) {
                        for (auto& [ID, GcActor] : m_selectChar_IDMapGcActor) {
                            const auto& actGraph = GcActor->GetGraph();
                            if (GcActor == nullptr)
                                continue;
                            if (DetectIntersection(cursorPt.x, cursorPt.y, actGraph->p()) ||
                                m_curSelCharIndex == m_selectChar_IDMapIndex[ID]) {
                                actGraph->SetBaseColor(highColor);
                            } else
                                actGraph->SetBaseColor(normalColor);
                        }
                    }
                }
            }
        }
    }
    //UI��Ⱦ
    g_workspace.Run(fSecond * 1000);
}

void GcLogin::OnBeginRender() {
    guard;
    unguard;
}

//lyymark 2.GcLogin.OnRun OnEndRender ��Ⱦ���� ��ӡ֡��ͳ����Ϣ
void GcLogin::OnEndRender() {
    guard;
    // ��ȡ֡ͳ����Ϣ
    const TCHAR* frameStats = GetDevice()->GetFrameStats();

    // ��ȡ���λ��
    int mouseX = g_workspace.GetMousePosX();
    int mouseY = g_workspace.GetMousePosY();

    // ��ʽ���ַ���
    TCHAR cTmp[128] = _T("\0");
    _stprintf_s(cTmp, _T("%s   MouseX: %d  MouseY: %d"), frameStats, mouseX, mouseY);

    // �����ַ���
    GetDevice()->DrawString(0, 0, 0xFF00FF00, cTmp);
    unguard;
}

//lyymark 2.GcLogin.ģ����Ⱦ
void GcLogin::OnRender(float fSecond) {
    guard;
    GameSetCursor();  //�������Ϊ��Ϸ���
    GetDevice()->SetLight(0, &m_lightDirect);
    if (m_bLight2) {
        GetDevice()->SetLight(1, &m_lightDirect2);
        GetDevice()->SetLight(2, NULL);
    } else {
        GetDevice()->SetLight(1, NULL);
    }
    if (m_eCurrentStatus == GLS_LOADING) {
        if (m_bLoading) {
            OnLoading();
            SetLoginState(GLS_SELECT_GAMEWORLD_SERVER);
        } else {
            m_bLoading = true;
        }
    }
    RtgMatrix16 m16;
    GetDevice()->SetMatrix(RTGTS_WORLD, &m16);
    GetDevice()->SetRenderState(RTGRS_Z_TEST, TRUE);
    OnRenderMask(RTGRM_NOBLENDOBJ_ONLY, fSecond);
    OnRenderMask(RTGRM_BLENDOBJ_ONLY, fSecond);
    unguard;
}

void GcLogin::OnRenderMask(RTGRenderMask mask, float fSecond) {
    guard;
    CRT_ActorInstance* pActor;
    RtgCamera*         pCamera = GetDevice()->m_pCamera;

    for (const auto& pair : m_mapActor) {
        // ��ȡָ�� Actor ʵ����ָ��
        pActor = pair.second;
        // �Ż���Ⱦ�߼�
        if (pCamera && !pCamera->CheckVisable(pActor->GetStaticBoundingBox()))
            continue;
        bool bFogChange = false;
        // �ж��Ƿ���Ҫ������Ч
        if (!m_lSkyFog && pair.first == "Sky") {
            bFogChange = true;
        }
        // �����Ҫ������Ч�������豸����
        if (bFogChange) {
            if (GetDevice()->GetFogEnable()) {
                GetDevice()->SetFogEnable(FALSE);
            } else {
                bFogChange = false;
            }
        }
        // ��Ⱦ Actor
        pActor->Render(GetDevice(), mask);
        // �ָ���Ч����
        if (bFogChange) {
            GetDevice()->SetFogEnable(TRUE);
        }
    }

    if (m_eCurrentStatus == GLS_SELECT_CHAR) {
        for (auto& [id, actor] : m_selectChar_IDMapGcActor) {
            if (actor)
                actor->Render(*GetDevice(), mask);
        }
    } else if (m_eCurrentStatus == GLS_CREATE_CHAR) {
        for (int i = 0; i < m_creatActorList.size(); i++) {
            pActor = m_creatActorList[i];
            if (pActor) {
                pActor->Render(GetDevice(), mask, true, true);
            }
        }
        if (m_pZsDao) {
            m_pZsDao->Render(GetDevice(), mask);
        }
        if (m_pZsDao2) {
            m_pZsDao2->Render(GetDevice(), mask);
        }
        if (m_pSsJian) {
            m_pSsJian->Render(GetDevice(), mask);
        }
        if (m_pSsJianPath) {
            m_pSsJianPath->Render(GetDevice(), mask);
        }
        if (m_pMmGong) {
            m_pMmGong->Render(GetDevice(), mask);
        }
        if (m_pDsLun) {
            m_pDsLun->Render(GetDevice(), mask);
        }
    }
    unguard;
}

void GcLogin::OnMouseMove(int iButton, int x, int y, int increaseX, int increaseY) {
    guard;
    if (!m_bCanInput)
        return;
    unguard;
}

void GcLogin::OnMouseWheel(int iButton, long vDelta) {
    guard;
#ifdef _DEBUG
    GetDevice()->m_pCamera->MoveForward(vDelta * 100.0f);
#endif
#ifdef _PREVIEW
    GetDevice()->m_pCamera->MoveForward(vDelta * 100.0f);
#endif
    if (!m_bCanInput)
        return;
    unguard;
}

void GcLogin::OnMouseLDown(int iButton, int x, int y) {
    guard;
    if (!m_bCanInput)
        return;
    if (g_workspace.IsMouseMonopolist())
        return;
    int         i = 0;
    RtgMatrix16 m16;
    RtgVertex3  vOrig, vDir, v0, v1, vMin, vMax;
    RtgAABB*    pAABB;
    float       r1, r2;
    GetDevice()->GetPickRayFromScreen(x, y, vOrig, vDir);
    switch (m_eCurrentStatus) {
        case GLS_SELECT_CHAR:
            if (!m_selectChar_IDMapGcActor.size())
                break;
            for (auto& [ID, GcActor] : m_selectChar_IDMapGcActor) {
                const auto& actGraph = GcActor->GetGraph();
                if (GcActor == nullptr)
                    continue;
                if (DetectIntersection(x, y, actGraph->p())) {
                    m_curSelCharIndex = m_selectChar_IDMapIndex[ID];
                    UpdateSelectChar();
                    break;
                }
            }
            break;
        case GLS_CREATE_CHAR:
            for (i = 0; i < m_creatActorList.size(); i++) {
                if (m_creatActorList[i] == NULL)
                    continue;
                m16.Set4X3Matrix(*(m_creatActorList[i])->GetWorldMatrix());
                m16.Invert();
                v0 = vOrig;
                v1 = vOrig + vDir * 1000.f;  // ֻ�ж�1000����Զ������

                pAABB = (m_creatActorList[i])->GetStaticBoundingBox();
                if (pAABB == NULL)
                    continue;

                v0 = v0 * m16;
                v1 = v1 * m16;
                vMin = pAABB->Min();
                vMax = pAABB->Max();
                if (rtgIsLineIntersectAABB(v0, v1, vMin, vMax, &r1, &r2)) {
                    SetSelectUser(i);
                    break;
                }
            }
            if (i == m_creatActorList.size())
                return;
            //���к󲥷����ﶯ��
            if (m_iCurSelectChar == 0) {
                const SRT_Pose* _pose = &m_creatActorList[0]->GetCurrentPose();
                if (_pose && _pose->Name != "waiting_login") {

                    if (m_creatActorList[0]) {
                        m_creatActorList[0]->PlayPose("waiting_login", false);
                    }
                }
            } else if (m_iCurSelectChar == 1) {
                const SRT_Pose* _pose = &m_creatActorList[1]->GetCurrentPose();
                if (_pose && _pose->Name != "waiting_login") {

                    if (m_creatActorList[1]) {
                        m_creatActorList[1]->PlayPose("waiting_login", false);
                    }
                }
                auto* t_body = FindModel("Body");
                t_body->PlayPose("left");

            } else if (m_iCurSelectChar == 2) {
                const SRT_Pose* _pose = &m_creatActorList[2]->GetCurrentPose();

                if (_pose && _pose->Name != "waiting_login") {

                    m_creatActorList[2]->PlayPose("waiting_login", false);
                }
                auto* t_body = FindModel("Body");
                t_body->PlayPose("right");

            } else if (m_iCurSelectChar == 3) {
                const SRT_Pose* _pose = &m_creatActorList[3]->GetCurrentPose();

                if (_pose && _pose->Name != "waiting_login") {

                    if (m_creatActorList[3]) {
                        m_creatActorList[3]->PlayPose("waiting_login", false);
                    }
                }
            }
            break;
    }
    unguard;
}

void GcLogin::OnMouseLDClick(int iButton, int x, int y) {
    guard;
    if (!m_bCanInput)
        return;
    unguard;
}

void GcLogin::OnMouseRDown(int iButton, int x, int y) {
    guard;
    if (!m_bCanInput)
        return;
    unguard;
}

void GcLogin::OnMouseRDrag(int iButton, int x, int y, int increaseX, int increaseY) {
    guard;
#if defined(_DEBUG) || defined(_PREVIEW)
    GetDevice()->m_pCamera->AddYaw(DegreeToRadian(increaseX));
    GetDevice()->m_pCamera->AddRoll(DegreeToRadian(-increaseY));
#endif
    if (!m_bCanInput)
        return;
    unguard;
}

void GcLogin::OnMouseMDrag(int iButton, int x, int y, int increaseX, int increaseY) {
    guard;
    if (!m_bCanInput)
        return;
    unguard;
}

//lyymark 2.GcLogin ��v��ʾ��ǰViewMatrix
void GcLogin::OnKeyDown(int iButton, int iKey) {
    guard;
#ifdef _PREVIEW
    P_LOGINFO("key down:" + std::to_string(iButton));
    if (iButton == 86) {
        auto gvm = GetDevice()->m_pCamera->GetViewMatrix();
        P_LOGINFO("ViewMatrix: \n" + gvm.ToString());
    }
#endif
    if (!m_bCanInput)
        return;
    unguard;
}

void GcLogin::OnKeyUp(int iButton, int iKey) {
    guard;
    if (!m_bCanInput)
        return;
    unguard;
}

void GcLogin::SelectGameWorld(int iIdx) {
    guard;
    if (iIdx < 0 || iIdx >= ms_pGameWorldServerList.size())
        return;
    m_szGameWorldServerName = ms_pGameWorldServerList[iIdx].szName;
    m_szGameWorldServerIP = ms_pGameWorldServerList[iIdx].szIP;
    m_lGameWorldServerPort = ms_pGameWorldServerList[iIdx].lPort;
    g_layerLogin->mp_selectServerName->SetText(m_szGameWorldServerName);  //��ʾ��ǰ����������
    SetLoginState(GLS_LOGIN);
    unguard;
}

void GcLogin::Login(const std::string& szUsername, const std::string& szPassword) {
    guard;
    m_szAccountUsername = szUsername;
    m_szAccountPassword = szPassword;
    if (m_szGameWorldServerIP.empty()) {
        ShowMessage(R(LMSG_CANNOT_FIND_GWS_IP));
    }
    if (!this->ConnectServer(m_szGameWorldServerIP.c_str(), m_lGameWorldServerPort)) {
        LoginErrMsg(GLEM_CANNOT_CONNECT);
    }
    unguard;
}

void GcLogin::NetClose() {
    guard;
    Close();
    unguard;
}

void GcLogin::OnNetLogin(int result, const char* szRetStr, short sRetCode, char cPointType,
                         long lPoint) {
    guard;
    LOG2("����: %s [%s].\n", R(G_LOGIN), result == LOGIN_RET_SUCCESS ? R(G_SUCCEED) : R(G_FAIL));

    if (!this->QueryAccountInfo()) {
        LoginErrMsg(GLEM_GET_ACCOUNT_FAIL);
    }

    if (result == LOGIN_RET_SUCCESS) {
        m_szAccountPassword = "";
        //tim.yang   pw��½
        m_hallName.clear();
        m_hallKey.clear();
        m_ishallLogin = false;
    } else {
        EErrMsg err;
        switch (result) {
            default:
                err = GLEM_LOGIN_FAIL;
                break;
            case LOGIN_RET_FAILED_UNKNOWN:
                err = GLEM_LOGIN_FAIL;
                break;  //��¼ʧ��
            case LOGIN_RET_FAILED_USER_ONLINE:
                err = GLEM_LOGIN_FAIL_USER_ONLINE;
                break;  //���ʺ����ڱ�ʹ��
            case LOGIN_RET_FAILED_ACCOUNT_STOP:
                err = GLEM_LOGIN_FAIL_ACCOUNT_STOP;
                break;  //���ʺű���ͣ
            case LOGIN_RET_FAILED_ACCOUNT_PAUSE:
                err = GLEM_LOGIN_FAIL_ACCOUNT_PAUSE;
                break;  //�������
            case LOGIN_RET_FAILED_ACCOUNT_WRONG5:
                err = GLEM_LOGIN_FAIL_ACCOUNT_WRONG5;
                break;  //�������������ʺ��������Ϊ�����û���Ϣ��ȫ�������ʺ�10����
            case LOGIN_RET_FAILED_POT_NOT_ENOUGH:
                err = GLEM_LOGIN_FAIL_POT_NOT_ENOUGH;
                break;  //�ʺŵ�������
            case LOGIN_RET_FAILED_PWD_WRONG:
                err = GLEM_LOGIN_FAIL_PWD_WRONG;
                break;  //��¼ʧ��,���������������֤���Ƿ���ȷ
            case LOGIN_RET_FAILED_NOT_FOUND:
                err = GLEM_LOGIN_FAIL_NOT_FOUND;
                break;  //��¼ʧ��,��ȥ��վ������ʺ�
            case LOGIN_RET_FAILED_SERVER_FULL:
                err = GLEM_LOGIN_FAIL_SERVER_FULL;
                break;  //����������
            case LOGIN_RET_FAILED_SERVER_STOP:
                err = GLEM_LOGIN_FAIL_SERVER_STOP;
                break;  //������ά����
            case LOGIN_RET_FAILED_NEW_CARD:
                err = GLEM_LOGIN_FAIL_NEW_CARD;
                break;
            case LOGIN_RET_FAILED_PUBWIN_CENTER:
                err = GLEM_LOGIN_FAIL_PUBWIN_CENTER;
                break;  //ͬ����Ա����ʧ��
            case LOGIN_RET_ACCOUNT_IS_LOCKER:
                err = GLEM_LOGIN_FAIL_IS_LOCKED;
                break;  //�ʺű�����
        }

        if (result == LOGIN_RET_FAILED_NEW_CARD || result == LOGIN_RET_FAILED_USER_ONLINE) {
            LOAD_UI("loginForm.txtPwd")->SetText("");
        }
        LoginErrMsg(err, szRetStr, sRetCode);
        m_bSelCharNetSucceed = true;
    }
    unguard;
}

void GcLogin::OnNetConnect(bool bSucceeded) {
    guard;
    if (!bSucceeded) {
        LoginErrMsg(GLEM_CONNECT_FAIL);
    } else {
        GcLoginSession::Login(m_szAccountUsername.c_str(), m_szAccountPassword.c_str(), 0,
                              m_ishallLogin, m_bForceLogin);
    }
    unguard;
}

//����ǿ�Ƶ�¼
void GcLogin::SetForceLogin(bool bForceLogin) {
    m_bForceLogin = bForceLogin;
}

void GcLogin::OnNetDisconnect() {
    guard;
    if (m_nDisconnectCause == 1) {
        ShowMessage(R(WMSG_DISCONNECT_MULTI_LOGIN));
    } else {
        if (!m_bSelCharNetSucceed) {
            LoginErrMsg(GLEM_DISCONNECT);
        }
    }
    unguard;
}

void GcLogin::OnNetKick(int cCause) {
    switch (cCause) {
        case 1:
            m_nDisconnectCause = 1;
            break;
        default:
            break;
    }
    SetLoginState(GLS_LOADING);
}

void GcLogin::OnConnect(bool bSucceeded) {
    guard;
    this->OnNetConnect(bSucceeded);
    unguard;
}

void GcLogin::OnDisconnect() {
    guard;
    OnNetDisconnect();
    unguard;
}

void GcLogin::OnNetGetAccountInfo() {
    guard;
    UpdateSelectChar();
    unguard;
}

void GcLogin::OnNetSelectCharDone() {
    m_bSelCharNetSucceed = true;
}

//�ı������ɫ����
void GcLogin::ChangeCharHair(bool bNext) {
    if (m_iCurSelectChar < 0)
        return;
    int                iCharIndex = GetCharIndexByActorID(m_selectUserCharIds[m_iCurSelectChar]);
    vector<SHeadModel> heads;
    if (!g_TableHeadModel.GetHeadModel(s_CreatActIds_FromCsv[iCharIndex], heads))
        return;

    if (m_HeadModelIndex[iCharIndex] > (int)heads.size() || m_HeadModelIndex[iCharIndex] < 0)
        m_HeadModelIndex[iCharIndex] = 0;

    int indexPre = m_HeadModelIndex[iCharIndex];

    if (bNext) {
        m_HeadModelIndex[iCharIndex]++;
        if (m_HeadModelIndex[iCharIndex] >= heads.size())
            m_HeadModelIndex[iCharIndex] = 0;
    } else {
        m_HeadModelIndex[iCharIndex]--;
        if (m_HeadModelIndex[iCharIndex] < 0)
            m_HeadModelIndex[iCharIndex] = heads.size() - 1;
    }

    m_creatActorList[m_iCurSelectChar]->UnloadSkin(heads[indexPre].skin.c_str());
    m_creatActorList[m_iCurSelectChar]->LoadSkin(heads[m_HeadModelIndex[iCharIndex]].skin.c_str(),
                                                 true);

    LOAD_UI("fmcreatid2.fmhair.lbhairname")->SetText(heads[m_HeadModelIndex[iCharIndex]].name);
    //LOAD_UI("fmcreatid2.fmface.lbfacename")->SetText(heads[m_HeadModelIndex[iCharIndex]].name);
}

/*----------------------------------------------------------------------------
-   Guide Net
----------------------------------------------------------------------------*/
class GcsGuideSession : public CG_TCPSession {
   public:
    GcsGuideSession(const char* szIP, int iPort) {
        m_szIP = szIP;
        m_iPort = iPort;
        m_fReConnectTime = 0.f;
        this->EnableCompress(false);
    }

    virtual ~GcsGuideSession() {}

    void ReConnect() {
        guard;
        if (m_szIP.IsEmpty()) {
            ShowMessage(R(LMSG_CANNOT_FIND_GS_IP));
        }
        Connect(m_szIP, m_iPort);
        m_fReConnectTime = 0.f;
        unguard;
    }

    void OnProcess(float fSecond) {
        guard;
        if (m_fReConnectTime > 0.f) {
            if (GetState() != NET_STATE_DEAD && GetState() != NET_STATE_ZERO) {
                Close();
            }
            m_fReConnectTime -= fSecond;
            if (m_fReConnectTime <= 0.f) {
                ReConnect();
            }
        } else {
            Process();
        }
        unguard;
    }

    virtual void OnConnect(bool ret) {
        guard;
        if (ret) {
            CG_CmdPacket sendCmd;
            sendCmd.BeginWrite();
            sendCmd.WriteShort(c2gs_query_gws_list);
            SendPacket(&sendCmd);
        } else {
            m_fReConnectTime = 10.f;
            // TODO: �������Ҫ��Ӧ������Ϸ��˵�����Ӳ��ϣ���Ҫ����
        }
        unguard;
    }

    virtual void OnReceive(CG_CmdPacket* packet) {
        guard;
        short sCmdID;
        if (packet->ReadShort(&sCmdID)) {
            //RtwWidget* pWidget = LOAD_UI("fmserver.fmxmm.lbserver"); gao
            if (sCmdID == gs2c_gws_list) {
                /*if (pWidget)
				{
					pWidget->SetText(R(LMSG_SELECT_SERVERS));
				}*/ // gao
                m_fReConnectTime = GcLogin::OnGuideGetGameWorldServerList(packet);
                if (m_fReConnectTime <= 0.f) {
                    m_fReConnectTime = 10.f;
                } else if (m_fReConnectTime > 60.f) {
                    m_fReConnectTime = 60.f;
                }
            } else {
                /*if (pWidget)
				{
					pWidget->SetText(R(LMSG_LOGIN_FAIL_SERVER_MAINTENANCE));
				}*/ // gao
                m_fReConnectTime = 10.f;
            }
        }
        unguard;
    }

    virtual void OnDisconnect() {
        m_fReConnectTime = 10.f;
        // TODO: �������Ҫ��Ӧ������Ϸ��˵�����Ӳ��ϣ���Ҫ����
    }

   private:
    RtString m_szIP;
    bool     m_bIsGetData;
    int      m_iPort;
    float    m_fReConnectTime;
};

CG_TCPSession*                      GcLogin::ms_pGuideNet = NULL;
std::vector<GcLogin::SGWServerList> GcLogin::ms_pGameWorldServerList;

void GcLogin::StartGetGameWorldServer() {
    guard;
    if (ms_pGuideNet == NULL) {
        char* szIP;
        int   iPort;
        GetGuideHostInfo(&szIP, &iPort);
        ms_pGuideNet = RT_NEW GcsGuideSession(szIP, iPort);
        ((GcsGuideSession*)ms_pGuideNet)->ReConnect();
    }
    unguard;
}

void GcLogin::EndGetGameWorldServer() {
    DEL_ONE(ms_pGuideNet);
}

void GcLogin::OnGuideNetProcess(float fSecond) {
    guard;
    if (ms_pGuideNet) {
        ((GcsGuideSession*)ms_pGuideNet)->OnProcess(fSecond);
    }
    unguard;
}

float GcLogin::OnGuideGetGameWorldServerList(CG_CmdPacket* pPacket) {
    guard;
    short sTimeSecond = 10, sCnt, sPort;
    char  cEvaluation;
    char* pName = NULL;
    char* pIP = NULL;
    // ��ȡ���´θ��µ�ʱ�䣬��ȡ�÷������б�
    pPacket->ReadShort(&sTimeSecond);
    pPacket->ReadShort(&sCnt);

    // ���� vector ��С
    ms_pGameWorldServerList.resize(sCnt);

    for (int i = 0; i < sCnt; ++i) {
        pPacket->ReadString(&pName);
        pPacket->ReadString(&pIP);
        pPacket->ReadShort(&sPort);
        pPacket->ReadByte(&cEvaluation);

        if (pName && pIP) {
            ms_pGameWorldServerList[i].lPort = sPort;
            ms_pGameWorldServerList[i].szName = pName;
            ms_pGameWorldServerList[i].szIP = pIP;
            ms_pGameWorldServerList[i].lEvaluation = cEvaluation;
            // ms_pGameWorldServerList[i].ping = GetPing(pIP);
        } else {
            // ���� pName �� pIP Ϊ�յ����
        }
    }
    if (GetLogin()) {
        GetLogin()->OnUIUpdateGameWorldServerList();
    }
    return sTimeSecond;
    unguard;
}

// ��������Ҫ����д
int GcLogin::GetPing(char* zIP) {
    guard;
    return 1;
    unguard;
}

//lyymark 2.GcLogin.UI.OnUIUpdateGameWorldServerList ������Ϸ����������б�
void GcLogin::OnUIUpdateGameWorldServerList() const {
    guard;
    if (m_eCurrentStatus != GLS_SELECT_GAMEWORLD_SERVER)
        return;
    std::string szEvaluation;
    int         tempConunt = 0;
    for (auto& gwServer : ms_pGameWorldServerList) {
        std::string szName = gwServer.szName;
        switch (gwServer.lEvaluation) {
            case -1:
                //strcpy(szEvaluation, "ά��");//by fox for string
                szEvaluation = R(MSG_SERVER_VINDICATE);
                break;
            case 0:  //    0
            case 1:  //  400
            case 2:  //  800
            case 3:  // 1200
            case 4:  // 1600
            case 5:  // 2000
            case 6:  // 2400
            case 7:  // 2800
            case 8:  // 3200
            default:
                szEvaluation = "����";
                break;
            case 9:  // 3600
                szEvaluation = "��æ";
                break;
            case 10:  // 4000 ������
                szEvaluation = "��Ա";
                break;
        }
        g_layerLogin->mp_layerServer->OnInsertNewServer(tempConunt++, szName, gwServer.ping,
                                                        szEvaluation);
    }
    unguard;
}

void GcLogin::OnPoseBegin(SRT_Pose* pose) {
    guard;
    //g_pMusicThread->Play("login_bg.ogg", true);
    if (m_eCurrentStatus == GLS_CREATE_CHAR) {
        LOAD_UI("lbbackground")->Hide();
        LOAD_UI("btnmale")->Hide();
        LOAD_UI("btnfemale")->Hide();
        LOAD_UI("fmcreatid1")->Hide();
        LOAD_UI("fmcreatid2")->Hide();
        LOAD_UI("fmcreatid3")->Hide();
        LOAD_UI("fmjob")->Hide();
        if (pose->Name == "otooh" || pose->Name == "stooh" || pose->Name == "wtooh" ||
            pose->Name == "mtooh") {
            LOAD_UI("fmhuajian.btnhuajian")->Enable();
            LOAD_UI("fmshushan.btnshushan")->Disable();
            LOAD_UI("fmshengwu.btnshengwu")->Disable();
            LOAD_UI("fmmingwang.btnmingwang")->Disable();
        } else if (pose->Name == "otoos" || pose->Name == "htoos" || pose->Name == "wtoos" ||
                   pose->Name == "mtoos") {
            LOAD_UI("fmhuajian.btnhuajian")->Disable();
            LOAD_UI("fmshushan.btnshushan")->Enable();
            LOAD_UI("fmshengwu.btnshengwu")->Disable();
            LOAD_UI("fmmingwang.btnmingwang")->Disable();
        } else if (pose->Name == "otoom" || pose->Name == "htoom" || pose->Name == "stoom" ||
                   pose->Name == "wtoom") {
            LOAD_UI("fmhuajian.btnhuajian")->Disable();
            LOAD_UI("fmshushan.btnshushan")->Disable();
            LOAD_UI("fmshengwu.btnshengwu")->Enable();
            LOAD_UI("fmmingwang.btnmingwang")->Disable();
        } else if (pose->Name == "otoow" || pose->Name == "htoow" || pose->Name == "stoow" ||
                   pose->Name == "mtoow") {
            LOAD_UI("fmhuajian.btnhuajian")->Disable();
            LOAD_UI("fmshushan.btnshushan")->Disable();
            LOAD_UI("fmshengwu.btnshengwu")->Disable();
            LOAD_UI("fmmingwang.btnmingwang")->Enable();
        }
    }
    unguard;
}

void GcLogin::OnPoseEnd(SRT_Pose* pose) {
    guard;
    iAnimalIndex = rand() % 12;
    iRandomAnimalIndex = iAnimalIndex;
    RtwImage* pImage = g_workspace.getImageFactory()->createImage("ui/textures/11.tga");
    RtwImage* pImage1 = g_workspace.getImageFactory()->createImage("ui/textures/11_a.tga");
    if (m_eCurrentStatus == GLS_CREATE_CHAR) {
        if (pose->Name == "otooh" || pose->Name == "stooh" || pose->Name == "wtooh" ||
            pose->Name == "mtooh") {
            g_pMusicThread->Play("bgm_005_a.ogg", true);
            LOAD_UI("lbbackground")->Show();
            LOAD_UI("btnmale")->Show();
            LOAD_UI("btnfemale")->Show();
            LOAD_UI("fmcreatid1")->Show();
            LOAD_UI("fmcreatid2")->Show();
            LOAD_UI("fmcreatid3")->Show();
            LOAD_UI("fmjob")->Show();
            if (pImage) {
                //SetSourceRect�Ĳ��������ϽǺ����½�������
                pImage->SetSourceRect(RtwRect(158, 249, 471, 399));
                pImage->SetBlend(true);
                LOAD_UI("fmjob.lbtxtjob")->SetBackgroundImage(pImage);
            }
            LOAD_UI("btnmale")->Disable();
            LOAD_UI("btnfemale")->Enable();
            LOAD_UI_T(CUiCheckButton, "btnfemale")->SetChecked(true);

            LOAD_UI("fmhuajian.btnhuajian")->Enable();
            LOAD_UI("fmshushan.btnshushan")->Enable();
            LOAD_UI("fmshengwu.btnshengwu")->Enable();
            LOAD_UI("fmmingwang.btnmingwang")->Enable();
            LOAD_UI_T(CUiCheckButton, "fmshushan.btnshushan")->SetChecked(false);
            LOAD_UI_T(CUiCheckButton, "fmhuajian.btnhuajian")->SetChecked(true);
            LOAD_UI_T(CUiCheckButton, "fmshengwu.btnshengwu")->SetChecked(false);
            LOAD_UI_T(CUiCheckButton, "fmmingwang.btnmingwang")->SetChecked(false);
            //Ĭ����ʾ����2Dͼ��
            pImage =
                g_workspace.getImageFactory()->createImage("ui_texture/pic_role_hj_f_01_f.dds");
            pImage->SetBlend(true);
            LOAD_UI("fmcreatid2.fmhead.lbpichead")->SetBackgroundImage(pImage);

            LOAD_UI("fmcreatid1.fmanimal.lbanimal")->SetBackgroundImage(pImage);
            //�������ﶯ��
            if (m_pMmGong) {
                m_pMmGong->PlayPose("login_s0", false);
            }

            CRT_ActorInstance* hjActor = GetSelectedActorByActorID(FengWu);
            if (hjActor) {
                hjActor->PlayPose("login_s0", false);
            }
        } else if (pose->Name == "otoos" || pose->Name == "htoos" || pose->Name == "wtoos" ||
                   pose->Name == "mtoos") {
            g_pMusicThread->Play("bgm_004_a.ogg", true);
            LOAD_UI("lbbackground")->Show();
            LOAD_UI("btnmale")->Show();
            LOAD_UI("btnfemale")->Show();
            LOAD_UI("fmcreatid1")->Show();
            LOAD_UI("fmcreatid2")->Show();
            LOAD_UI("fmcreatid3")->Show();
            LOAD_UI("fmjob")->Show();
            if (pImage) {
                //SetSourceRect�Ĳ��������ϽǺ����½�������
                pImage->SetSourceRect(RtwRect(160, 5, 505, 240));
                pImage->SetBlend(true);
                LOAD_UI("fmjob.lbtxtjob")->SetBackgroundImage(pImage);
            }
            LOAD_UI("btnmale")->Enable();
            LOAD_UI_T(CUiCheckButton, "btnmale")->SetChecked(true);
            LOAD_UI("btnfemale")->Disable();

            LOAD_UI("fmhuajian.btnhuajian")->Enable();
            LOAD_UI("fmshushan.btnshushan")->Enable();
            LOAD_UI("fmshengwu.btnshengwu")->Enable();
            LOAD_UI("fmmingwang.btnmingwang")->Enable();
            LOAD_UI_T(CUiCheckButton, "fmhuajian.btnhuajian")->SetChecked(false);
            LOAD_UI_T(CUiCheckButton, "fmshushan.btnshushan")->SetChecked(true);
            LOAD_UI_T(CUiCheckButton, "fmshengwu.btnshengwu")->SetChecked(false);
            LOAD_UI_T(CUiCheckButton, "fmmingwang.btnmingwang")->SetChecked(false);
            //Ĭ����ʾ����2Dͼ��
            pImage =
                g_workspace.getImageFactory()->createImage("ui_texture/pic_role_ss_m_01_f.dds");
            pImage->SetBlend(true);
            LOAD_UI("fmcreatid2.fmhead.lbpichead")->SetBackgroundImage(pImage);

            //�������ﶯ��
            m_pSsJianPath->PlayPose("login_l0", false);

            CRT_ActorInstance* shActor = GetSelectedActorByActorID(ShuShi);
            if (shActor)
                shActor->PlayPose("login_l0", false);
        } else if (pose->Name == "otoom" || pose->Name == "htoom" || pose->Name == "stoom" ||
                   pose->Name == "wtoom") {
            g_pMusicThread->Play("bgm_015_a.ogg", true);
            LOAD_UI("lbbackground")->Show();
            LOAD_UI("btnmale")->Show();
            LOAD_UI("btnfemale")->Show();
            LOAD_UI("fmcreatid1")->Show();
            LOAD_UI("fmcreatid2")->Show();
            LOAD_UI("fmcreatid3")->Show();
            LOAD_UI("fmjob")->Show();
            if (pImage1) {
                //SetSourceRect�Ĳ��������ϽǺ����½�������
                pImage1->SetSourceRect(RtwRect(161, 250, 510, 480));
                pImage1->SetBlend(true);
                LOAD_UI("fmjob.lbtxtjob")->SetBackgroundImage(pImage1);
            }
            LOAD_UI("btnmale")->Disable();
            LOAD_UI("btnfemale")->Enable();
            LOAD_UI_T(CUiCheckButton, "btnfemale")->SetChecked(true);

            LOAD_UI("fmhuajian.btnhuajian")->Enable();
            LOAD_UI("fmshushan.btnshushan")->Enable();
            LOAD_UI("fmshengwu.btnshengwu")->Enable();
            LOAD_UI("fmmingwang.btnmingwang")->Enable();
            LOAD_UI_T(CUiCheckButton, "fmshushan.btnshushan")->SetChecked(false);
            LOAD_UI_T(CUiCheckButton, "fmhuajian.btnhuajian")->SetChecked(false);
            LOAD_UI_T(CUiCheckButton, "fmshengwu.btnshengwu")->SetChecked(true);
            LOAD_UI_T(CUiCheckButton, "fmmingwang.btnmingwang")->SetChecked(false);
            //Ĭ����ʾ����2Dͼ��
            pImage =
                g_workspace.getImageFactory()->createImage("ui_texture/pic_role_mj_f_01_f.dds");
            pImage->SetBlend(true);
            LOAD_UI("fmcreatid2.fmhead.lbpichead")->SetBackgroundImage(pImage);
            //�������ﶯ��
            if (m_pDsLun) {
                m_pDsLun->PlayPose("login_z0", false);
            }
            CRT_ActorInstance* mjActor = GetSelectedActorByActorID(DaoShi);
            if (mjActor)
                mjActor->PlayPose("login_z0", false);
        } else if (pose->Name == "otoow" || pose->Name == "htoow" || pose->Name == "stoow" ||
                   pose->Name == "mtoow") {
            g_pMusicThread->Play("bgm_014_a.ogg", true);
            LOAD_UI("lbbackground")->Show();
            LOAD_UI("btnmale")->Show();
            LOAD_UI("btnfemale")->Show();
            LOAD_UI("fmcreatid1")->Show();
            LOAD_UI("fmcreatid2")->Show();
            LOAD_UI("fmcreatid3")->Show();
            LOAD_UI("fmjob")->Show();
            if (pImage1) {
                //SetSourceRect�Ĳ��������ϽǺ����½�������
                pImage1->SetSourceRect(RtwRect(168, 6, 511, 236));
                pImage1->SetBlend(true);
                LOAD_UI("fmjob.lbtxtjob")->SetBackgroundImage(pImage1);
            }
            LOAD_UI("btnmale")->Enable();
            LOAD_UI_T(CUiCheckButton, "btnmale")->SetChecked(true);
            LOAD_UI("btnfemale")->Disable();

            LOAD_UI("fmhuajian.btnhuajian")->Enable();
            LOAD_UI("fmshushan.btnshushan")->Enable();
            LOAD_UI("fmshengwu.btnshengwu")->Enable();
            LOAD_UI("fmmingwang.btnmingwang")->Enable();
            LOAD_UI_T(CUiCheckButton, "fmhuajian.btnhuajian")->SetChecked(false);
            LOAD_UI_T(CUiCheckButton, "fmshushan.btnshushan")->SetChecked(false);
            LOAD_UI_T(CUiCheckButton, "fmshengwu.btnshengwu")->SetChecked(false);
            LOAD_UI_T(CUiCheckButton, "fmmingwang.btnmingwang")->SetChecked(true);
            //Ĭ����ʾ����2Dͼ��
            pImage =
                g_workspace.getImageFactory()->createImage("ui_texture/pic_role_wt_m_01_f.dds");
            pImage->SetBlend(true);
            LOAD_UI("fmcreatid2.fmhead.lbpichead")->SetBackgroundImage(pImage);
            //�������ﶯ��
            if (m_pZsDao) {
                m_pZsDao->PlayPose("login_f0", false);
            }
            CRT_ActorInstance* wtActor = GetSelectedActorByActorID(ZhanShi);
            if (wtActor)
                wtActor->PlayPose("login_f0", false);
        }
    }
    unguard;
}

void GcLogin::OnPoseEvent(SRT_Pose* pose, SRT_PoseEvent* event) {}

void GcLogin::OnPoseBreak(SRT_Pose* oldPose, SRT_Pose* newPose) {}

int GcLogin::GetCharIndexByActorID(short actorID) {

    for (size_t i = 0; i < sizeof(s_CreatActIds_FromCsv) / sizeof(s_CreatActIds_FromCsv[0]); ++i) {
        if (s_CreatActIds_FromCsv[i] == actorID)
            return i;
    }
    return -1;
}

CRT_ActorInstance* GcLogin::GetSelectedActorByActorID(short actorID) {
    for (size_t index = 0; index < m_selectUserCharIds.size(); ++index) {
        if (m_selectUserCharIds[index] == actorID)
            return m_creatActorList[index];
    }
    return NULL;
}

// ��װ��������߼�⺯��     add by lyy 2024.8.5
// x,y����������   actor��Ҫ��������  �������ж������Ƿ������
bool GcLogin::DetectIntersection(const int& x, const int& y, CRT_ActorInstance* actor) {
    if (actor == nullptr)
        return false;
    const RtgAABB* pAABB = actor->GetStaticBoundingBox();  // ��ȡAABB
    if (pAABB == nullptr)
        return false;
    RtgVertex3  vOrig, vDir, v0, v1, vMin, vMax;
    float       r1, r2;
    RtgMatrix16 m16;
    GetDevice()->GetPickRayFromScreen(x, y, vOrig, vDir);  // ��ȡ����
    m16.Set4X3Matrix(*actor->GetWorldMatrix());            // ���þ�������
    m16.Invert();
    v0 = vOrig * m16;                         // ת�����ߵ�����ֲ�����ϵ
    v1 = v1 = (vOrig + vDir * 1000.f) * m16;  // ֻ�ж�1000����Զ������
    vMin = pAABB->Min();
    vMax = pAABB->Max();
    // �ж�������AABB�Ƿ��ཻ
    return rtgIsLineIntersectAABB(v0, v1, vMin, vMax, &r1, &r2);
}

/*----------------------------------------------------------------------------
-   End.
----------------------------------------------------------------------------*/

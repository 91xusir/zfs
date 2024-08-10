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
#include <graph/rtg_matrix.h>
#include <string>
#include <cstdio>
#include <type_traits>
#include <gc_frame.h>
#include "PreConsole.h"
#include "GlobalConfig.h"
/*
lyy  2024 8.1-8.4  �ع� ���ע��  ɾ��Լ1500����������
*/
//���⾲̬������ʼ��
std::string GcLogin::m_szAccountUsername = "";  //�˻���
std::string GcLogin::m_szAccountPassword = "";  //����

GcLogin::GcLogin(CGameClientFrame* pGameClientFrame) : m_ini(true) {
    m_pGameClientFrame = pGameClientFrame;  //�ͻ��˾����
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
    pIni->FindSection(szSectionName.c_str());
    if (pIni->FirstEntry(&szLink, &szName)) {
        do {
            pActor = RtcGetActorManager()->CreateActor(szName, true);  // ����һ��actorʵ��
            if (!pActor)
                continue;
            // �������CreateChar���򲥷Ŷ��� CreateChar���浥������
            if (szSectionName != "CreateChar") {
                pActor->PlayPose(true);
            }
            mapActor[std::string(szLink)] = pActor;  // ��actorʵ����ӵ�ӳ����
            if (szLink == "Body") {  // �����������Ϊ"Body"���򽫵�ǰactorʵ����Ϊ����
                pBody = pActor;
            } else {
                if (!pBody)
                    continue;
                pActor->LinkParent(pBody, szLink.c_str());  // ����pBody
            }

            if (pActor->m_Name == "pn01.act" || pActor->m_Name == "pt01.act" ||
                pActor->m_Name == "ph01.act" || pActor->m_Name == "pf01.act") {
                auto& m_poseMap             = pActor->GetCore()->m_poseMap;
                m_poseMap["waiting_n0"]     = m_poseMap["waiting_non"];
                m_poseMap["waiting_n1"]     = m_poseMap["waiting_non"];
                m_poseMap["run_n0"]         = m_poseMap["walk_non"];
                m_poseMap["wait_n0"]        = m_poseMap["wait_non"];
                m_poseMap["wait_a_n0"]      = m_poseMap["wait_non"];
                m_poseMap["attack_n0"]      = m_poseMap["attack_non"];
                m_poseMap["attack_n0"].Name = "attack_n0";
                m_poseMap["attack_n1"]      = m_poseMap["attack_non"];
                m_poseMap["attack_n1"].Name = "attack_n1";
                m_poseMap["attack_n2"]      = m_poseMap["attack_non"];
                m_poseMap["attack_n2"].Name = "attack_n2";
                m_poseMap["attack_l0"]      = m_poseMap["attack_non_flysword"];
                m_poseMap["attack_l1"]      = m_poseMap["attack_non_flysword"];
                m_poseMap["attack_l2"]      = m_poseMap["attack_non_flysword"];
                m_poseMap["hurt_n0"]        = m_poseMap["hurt_non"];
                m_poseMap["critical_n0"]    = m_poseMap["attack_non"];
            }
            if (pActor->m_Name == "pn01.act") {
                auto& m_poseMap = pActor->GetCore()->m_poseMap;
                for (auto& [name, pose] : m_poseMap) {

                    P_LOGINFO(name);
                }
            }
        } while (pIni->NextEntry(&szLink, &szName));
    }
    unguard;
}

bool GcLogin::InitOnce() {
    return true;
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
    for (auto it = m_selRole_IDMapGcActor.begin(); it != m_selRole_IDMapGcActor.end();) {
        DEL_ONE(it->second);
        it = m_selRole_IDMapGcActor.erase(it);
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
    auto it = m_mapActor.find(szName);
    if (it != m_mapActor.end())
        return it->second;
    return nullptr;
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

//����scene/login.ini �������¼����
void GcLogin::OnRenderLoading() {
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

//����loadingҳ�����
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

void GcLogin::EnterSelectGameWorldServer() {
    DXUtil_Timer(TIMER_RESET);  //���ڽ��֡�����ƿ��ټ���
    if (g_layerLogin == nullptr) {
        UILayer::EnterLogin();
    }
    StartGetGameWorldServer();             //��ʼ��ȡ��Ϸ���������
    g_layerLogin->mp_layerServer->Show();  //��ʾ�������б�
}

bool GcLogin::LeaveSelectGameWorldServer() const {
    CHECK(m_eCurrentStatus == GLS_SELECT_GAMEWORLD_SERVER);
    g_layerLogin->mp_layerServer->Hide();
    EndGetGameWorldServer();  //������ȡ��Ϸ���������
    return true;
}

void GcLogin::ReadAccountFromFile() {
    GlobalConfig config;  // ���� �������������� ����
    if (config.OpenFile(R(INI_USER))) {
        const bool isSave = config["account"]["save"].at<bool>();
        g_layerLogin->mp_ckSaveAcc->SetChecked(isSave);
        if (isSave) {
            m_szAccountUsername = config["login"]["username"].at<std::string>();
            m_szAccountPassword = config["login"]["password"].at<std::string>();
        }
    }
    // ��ȡ�û���
    /* RtIni       iniUser;
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
    }*/

    g_layerLogin->mp_txtAccout->Enable();
    g_layerLogin->mp_txtPwd->Enable();
    g_layerLogin->mp_txtAccout->SetText(m_szAccountUsername);
    g_layerLogin->mp_txtPwd->SetText(m_szAccountPassword);
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

bool GcLogin::LeaveLogin() const {
    guard;
    CHECK(m_eCurrentStatus == GLS_LOGIN);
    g_layerLogin->Hide();
    return true;
    unguard;
}

void GcLogin::EnterSelectChar() {
    guard;
    DXUtil_Timer(TIMER_RESET);  //���ڽ��֡�����ƿ��ټ���
    if (g_layerSelectChar == nullptr) {
        UILayer::EnterSelectChar();
    }
    LOAD_UI("roleBtn")->Show();
    LOAD_UI("enterBtn")->Show();
    LOAD_UI("delRoleBtn")->Show();
    LOAD_UI("btnsetcharpwd")->Show();
    LOAD_UI("btnsetcharpwd")->Disable();  //��ֹ���ý�ɫ���� ��Ϊ����д��
    LOAD_UI("rmbBtn")->Show();
    LOAD_UI("rmbBtn")->Disable();  //��ֹ��ֵ���� ��Ϊ����д��
    LOAD_UI("RoleInfo")->Hide();
    m_bCanInput = true;
    //m_curSelRoleIndex = -1;//����Ҫ����ѡ��״̬����Ȼÿ�ζ�Ҫ����ѡ��
    UpdateSelectChar();
    unguard;
}

void GcLogin::UpdateSelectChar() {
    guard;
    // ��鵱ǰ״̬�Ƿ�Ϊѡ���ɫ״̬�������򷵻�
    if (m_eCurrentStatus != GLS_SELECT_CHAR)
        return;
    m_selRole_IDMapIndex.clear();
    //��ȡ�˺���Ϣ
    const GcAccountInfo& accountInfo = GetAccountInfo();

    if (accountInfo.chatCount == 0 || m_curSelRoleIndex == -1) {  //�����ɫ����Ϊ0��û��ѡ�н�ɫ
        LOAD_UI("enterBtn")->Disable();                           //��ֹ������Ϸ
        LOAD_UI("delRoleBtn")->Disable();                         //��ֹɾ����ɫ
    }
    if (accountInfo.chatCount < 3)
        LOAD_UI("crtRoleBtn")->Enable();
    else
        LOAD_UI("crtRoleBtn")->Disable();

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

    GcActor*          pGcActor         = nullptr;
    const SUserActor* pUserActorInCsv  = nullptr;
    const SCreModel*  pActorModelInCsv = nullptr;
    // �����û���ɫ�б�,������Ӧģ�ͣ��Ѿ������Ļ�����
    for (std::size_t i = 0; i < accountInfo.chatCount; i++) {
        auto& userInfo = accountInfo.users[i];
        if (m_selRole_IDMapGcActor.find(userInfo.id) != m_selRole_IDMapGcActor.end())
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
        pGcActor->m_core.Sex    = userInfo.attributes.Sex;
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
            m_selRole_IDMapGcActor[userInfo.id] = pGcActor;
    }
    //���¿�ѡ��ɫ�б�
    for (std::size_t index = 0; index < accountInfo.chatCount; index++) {
        const auto& userInfo = accountInfo.users[index];
        if (m_selRole_IDMapGcActor.find(userInfo.id) != m_selRole_IDMapGcActor.end()) {
            m_selRole_IDMapIndex[userInfo.id] = index;
        }
    }
    if (!m_pBody)
        return;
    RtgMatrix12 _SlotMatrix;
    size_t      maxCount = std::min(accountInfo.chatCount, (long)UILayerSelectChar::MaxUserCharBtn);
    //ʵ����ֻ��ɾ������ӽ�ɫ����Ҫ���¸���λ�ã���������д�ж��� by lyy 2024.8.5
    for (std::size_t i = 0; i < maxCount; ++i) {
        auto&       user     = accountInfo.users[i];
        std::string boneName = "bno" + std::to_string(i + 1);
        auto*       gcActor  = m_selRole_IDMapGcActor[user.id];
        gcActor->mBaseActor.GetGraph()->p()->LinkParent(m_pBody, boneName.c_str());
        std::string roleName = user.name;
        SUserActor* pActor   = g_TableUserActor.FindUserActor(user.attributes.metier,  //ְҵ
                                                              user.attributes.metierLevel);  //��ת
        if (!pActor)
            P_LOGERROR("user_actor���ȡ����ְҵΪ��" + user.attributes.metier);
        //���ö�Ӧ�����ϵĽ�ɫ����
        g_layerSelectChar->m_charBtnArray[i]->SetText(roleName);
        //����ǵ�ǰѡ��Ľ�ɫ
        if (i == m_curSelRoleIndex) {
            g_layerSelectChar->m_charBtnArray[m_curSelRoleIndex]->SetTextColor(0xffff0000);  //��ɫ
            LOAD_UI("enterBtn")->Enable();
            LOAD_UI("delRoleBtn")->Enable();
            std::string Lev     = "Lv: " + std::to_string(user.attributes.level);
            std::string Faction = g_layerSelectChar->m_faction[user.attributes.faction];
            std::string Miter   = pActor->MetierName;
            LOAD_UI("RoleInfo")->Show();
            LOAD_UI("RoleInfo.RoleName")->SetText(roleName);
            LOAD_UI("RoleInfo.Lev")->SetText(Lev);
            LOAD_UI("RoleInfo.Faction")->SetText(Faction);
            LOAD_UI("RoleInfo.Miter")->SetText(Miter);
            /*     P_LOGINFO("��ǰѡ��Ľ�ɫidΪ:" + std::to_string(user.id) +
                      "������Ϊ :" + std::to_string(m_curSelRoleIndex));*/
        }
    }
    unguard;
}

void GcLogin::ClearSelectRoleOnce() {
    for (auto it = m_selRole_IDMapGcActor.begin(); it != m_selRole_IDMapGcActor.end();) {
        DEL_ONE(it->second);
        it = m_selRole_IDMapGcActor.erase(it);
    }
}

bool GcLogin::LeaveSelectChar() {
    guard;
    CHECK(m_eCurrentStatus == GLS_SELECT_CHAR);
    m_bCanInput = false;
    LOAD_UI("roleBtn")->Hide();
    LOAD_UI("enterBtn")->Hide();
    LOAD_UI("delRoleBtn")->Hide();
    LOAD_UI("btnsetcharpwd")->Hide();
    LOAD_UI("rmbBtn")->Hide();
    LOAD_UI("RoleInfo")->Hide();
    m_selectUserCharIds.clear();
    return true;
    unguard;
}

// --------------------������ɫ---------------------  add by lyy 2024.8.3
bool GcLogin::LoadModel(const std::string& modelName, CRT_ActorInstance** model,
                        std::string linkName /*= ""*/, CRT_ActorInstance** parent /*= nullptr*/) {
    if (*model == nullptr) {
        *model = RtcGetActorManager()->CreateActor(modelName.c_str(), true);
        if (*model == nullptr) {

            P_LOGWARN("[GcActor::Render] �޷���������ģ��:" + modelName);
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
    if (g_layerSelectChar == nullptr) {
        UILayer::EnterSelectChar();
    }
    //��ʼ����
    if (auto door = FindModel("Door")) {
        door->RealUseFrame(13);  //��ʼ���ŵ�״̬Ϊ��13֡  �ر�״̬
    }
    m_bCanInput = true;
    LOAD_UI("fmShangZhou")->Show();
    LOAD_UI("fmCreatRole")->Hide();
    m_curCrtRoleCsvID = -1;
    UpdateCreateChar();
    unguard;
}

void GcLogin::SetSelectShangOrZhou(int iSei) {
    //��ʼ��ѡ��ɫΪ��
    m_curCrtRoleCsvID = -1;
    m_selectFaction   = iSei;
    auto* door        = FindModel("Door");
    if (!door || !m_pBody)
        return;
    if (iSei == None) {
        door->PlayPose("close");  //����
        LOAD_UI("fmShangZhou")->Show();
        LOAD_UI("fmCreatRole")->Hide();
        return;
    }
    if (iSei != Shang && iSei != Zhou)
        return;
    auto* Tray_Shang = FindModel("Tray_Shang");
    auto* Tray_Zhou  = FindModel("Tray_Zhou");
    auto* BG_Shang   = FindModel("BG_Shang");
    auto* BG_Zhou    = FindModel("BG_Zhou");
    if (!Tray_Shang || !Tray_Zhou || !BG_Shang || !BG_Zhou || !m_pBody)
        return;
    Tray_Shang->m_bDisplay = iSei == Shang;
    BG_Shang->m_bDisplay   = iSei == Shang;
    Tray_Zhou->m_bDisplay  = iSei == Zhou;
    BG_Zhou->m_bDisplay    = iSei == Zhou;
    if (iSei == Shang) {
        Tray_Shang->LinkParent(m_pBody, "btray");
        BG_Shang->LinkParent(m_pBody, "bbg");
    } else {
        Tray_Zhou->LinkParent(m_pBody, "btray");
        BG_Zhou->LinkParent(m_pBody, "bbg");
    }
    UpdateCreateChar();
    //����
    LOAD_UI("fmShangZhou")->Hide();
    door->PlayPose("open");
}

bool GcLogin::LeaveCreateChar() {
    guard;
    m_bCanInput = false;
    LOAD_UI("fmShangZhou")->Hide();
    LOAD_UI("fmCreatRole")->Hide();
    return true;
    unguard;
}

void GcLogin::UpdateCreateChar() {
    if (!m_pBody)
        return;
    if (m_curCrtRoleCsvID < 0) {
        LOAD_UI("fmCreatRole.txtJob")->SetText("��");
        LOAD_UI("fmCreatRole.lbJobInfo")->SetText("��");
        LOAD_UI("crtRoleBtn")->Disable();
        LOAD_UI("fmCreatRole")->Hide();
    } else {
        const auto& actInfo = g_TableUserActor.FindUserActor(m_curCrtRoleCsvID);
        LOAD_UI("fmCreatRole.txtJob")->SetText(actInfo->MetierName);
        LOAD_UI("fmCreatRole.txtHead")->SetText(m_crtRole_csvIdMapHeads[m_curCrtRoleCsvID][0].name);
        LOAD_UI("fmCreatRole.lbJobInfo")->SetText(actInfo->Desc);
        LOAD_UI("crtRoleBtn")->Enable();
        LOAD_UI("fmCreatRole")->Show();
    }

    CRT_ActorInstance* pActor;
    const SUserActor*  p_userActCsv;
    RtgMatrix12        WeaponMatrix;
    RtgMatrix16        WorldMatrix;
    //��� m_crtRole_csvIdMapActIns Ϊ�ղ���Ҫ������ɫģ��
    if (m_crtRole_csvIdMapActIns.size() == 0) {
        for (const auto& csvID : s_CreatActIds_FromCsv) {
            const auto& usrActData = s_mCharSkinInfo.at(csvID);
            p_userActCsv           = g_TableUserActor.FindUserActor(csvID);
            if (!p_userActCsv || (!p_userActCsv->bZCreate && !p_userActCsv->bSCreate))
                continue;
            if (m_crtRole_csvIdMapActIns.find(csvID) != m_crtRole_csvIdMapActIns.end())
                continue;
            pActor = FindModel(usrActData.act);
            if (!pActor)
                continue;
            pActor->LoadSkin(usrActData.body.c_str());
            pActor->LoadSkin(usrActData.hand.c_str());
            pActor->LoadSkin(usrActData.leg.c_str());
            if (g_TableHeadModel.GetHeadModel(csvID, m_crtRole_csvIdMapHeads[csvID])) {
                pActor->LoadSkin(m_crtRole_csvIdMapHeads[csvID][0].skin.c_str(), true);
            }
            switch (csvID) {
                case ZhanShi:
                    LoadModel("iw0401dao_04", &m_pZsDao, "Box01", &pActor);
                    LoadModel("iw0401dao_04", &m_pZsDao2, "Box02", &pActor);
                    break;
                case FengWu:
                    LoadModel("iw0401gong_04", &m_pMmGong, "Box01", &pActor);
                    break;
                case ShuShi:
                    LoadModel("ss_fj", &m_pSsJianPath);
                    LoadModel("iw0501jian_04", &m_pSsJian, "Blink0", &m_pSsJianPath);
                    WeaponMatrix = WorldMatrix;
                    WeaponMatrix._32 += 15.f;
                    m_pSsJianPath->SetMatrix(WeaponMatrix);
                    m_pSsJianPath->LinkParent(pActor);
                    break;
                case DaoShi:
                    LoadModel("ss_fl", &m_pDsLunPath);
                    LoadModel("w_ssl_m_04_03", &m_pDsLun, "Blink0", &m_pDsLunPath);
                    WeaponMatrix = WorldMatrix;
                    m_pDsLunPath->SetMatrix(WeaponMatrix);
                    m_pDsLunPath->LinkParent(pActor);
                    break;
            }
            m_crtRole_csvIdMapActIns[csvID] = pActor;
            //ɾ��m_mapActor��������  ���������ֿ���Ⱦ
            if (auto it = m_mapActor.find(usrActData.act); it != m_mapActor.end()) {
                m_mapActor.erase(it);
            }
        }
    }
    int charAct[4]{};
    charAct[0] = ZhanShi;
    charAct[1] = FengWu;
    charAct[2] = m_selectFaction == Shang ? ShuShi : DaoShi;
    charAct[3] = m_selectFaction == Shang ? DaoShi : ShuShi;
    m_crtRole_csvIdMapActIns.at(charAct[2])->m_bDisplay = true;
    m_crtRole_csvIdMapActIns.at(charAct[3])->m_bDisplay = false;
    m_pSsJian->m_bDisplay                               = m_selectFaction == Shang;
    m_pDsLun->m_bDisplay                                = m_selectFaction == Zhou;
    for (std::size_t i = 0; i < 3; i++) {
        auto  soltName = "bno" + std::to_string(actSoltInTray[i]);
        auto& it       = m_crtRole_csvIdMapActIns.find(charAct[i]);
        if (it != m_crtRole_csvIdMapActIns.end()) {
            auto* ActIns = it->second;
            if (ActIns) {
                ActIns->LinkParent(m_pBody, soltName.c_str());
            }
        }
    }
    m_crtRole_csvIdMapActIns.at(charAct[3])->LinkParent(m_pBody, "bbg");
}

void GcLogin::ChangeCharHead(int charCsvId, int headIndex) {}

// -------------------��ɫ����end------------------

void GcLogin::UpdateGraphConfig(const char* szName) {
    float fCameraFOV    = 45.0f;       // ��������ӳ��Ƕȣ�Field of View������λΪ��
    float fCameraAspect = 4.f / 3.f;   // ��Ⱦ�ӿڵĿ�߱ȣ�����4:3
    float fCameraNear   = 10.0f;       //������
    float fCameraFar    = 30000.0f;    //Զ����
    float fFogNear      = 1000.f;      //��Ч����ʼ�ľ���
    float fFogFar       = 8000.f;      //��Ч�������ľ���
    long  lFogEnable    = 0;           //��Ч������
    long  lSkyLight     = 0xFF888888;  //��չ����ɫ����������Ϊ���ɫ
    float fLightR       = 70.f;        //����Դ�ĺ�ɫ����ǿ��
    float fLightW       = 0.f;         //����Դ�Ŀ�ȣ�0��ʾ���Դ
    float fLight2R      = 70.f;        //������Դ�ĺ�ɫ����ǿ��
    float fLight2W      = 0.f;         //������Դ�Ŀ�ȣ�0��ʾ���Դ

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
        m_bLight2                = true;
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

//�����ɫ��ת ��ʱû�� ����
void GcLogin::OnLeftRotation() {
    // m_selectUserCharActs[m_iCurSelectChar]->GetGraph()->p()->RotateLZ(DegreeToRadian(30.f));
}

//static int mRotationTime = 0;

void GcLogin::OnLeftRotationDown() {
    //mRotationTime++;
    // m_selectUserCharActs[m_iCurSelectChar]->GetGraph()->p()->RotateLZ((mRotationTime * 30) % 360);
}

void GcLogin::OnLeftRotationUp() {}

void GcLogin::OnRightRotation() {
    //m_selectUserCharActs[m_iCurSelectChar]->GetGraph()->p()->RotateLZ(DegreeToRadian(-30.f));
}

void GcLogin::SelectGameWorld(int iIdx) {
    guard;
    if (iIdx < 0 || iIdx >= ms_pGameWorldServerList.size())
        return;
    m_szGameWorldServerName = ms_pGameWorldServerList[iIdx].szName;
    m_szGameWorldServerIP   = ms_pGameWorldServerList[iIdx].szIP;
    m_lGameWorldServerPort  = ms_pGameWorldServerList[iIdx].lPort;
    g_layerLogin->mp_selectServerName->SetText(m_szGameWorldServerName);  //��ʾ��ǰ����������
    SetLoginState(GLS_LOGIN);
    unguard;
}

//������Ϸ
void GcLogin::OnSelectUserDone() {
    guard;
    if (m_curSelRoleIndex >= 0 && m_curSelRoleIndex < UILayerSelectChar::MaxUserCharBtn) {
        UILayer::EnterLoading();
        SelectChar(GetAccountInfo().users[m_curSelRoleIndex].id);
    } else {
        ShowMessage(R(LMSG_PLS_CHOOSE_CHAR));
    }
    unguard;
}

void GcLogin::OnSelectUserWithPwd() {
    UILayer::EnterLoading();
    SelectChar(GetAccountInfo().users[m_curSelRoleIndex].id);
}

void GcLogin::OnRestoreChar() {
    guard;
    if (m_curSelRoleIndex >= 0 && m_curSelRoleIndex < UILayerSelectChar::MaxUserCharBtn) {
        RestoreChar(GetAccountInfo().users[m_curSelRoleIndex].id);
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
    if (m_curSelRoleIndex >= 0 && m_curSelRoleIndex < UILayerSelectChar::MaxUserCharBtn) {
        DeleteChar(GetAccountInfo().users[m_curSelRoleIndex].id);
    } else {
        ShowMessage(R(LMSG_PLS_CHOOSE_CHAR));
    }
    unguard;
}

void GcLogin::OnSetCharPassword(const char* password) {
    guard;
    if (m_curSelRoleIndex >= 0 && m_curSelRoleIndex < UILayerSelectChar::MaxUserCharBtn) {
        SetCharPassword(GetAccountInfo().users[m_curSelRoleIndex].id, password);
    }
    unguard;
}

void GcLogin::OnChangeCharPassword(const char* old, const char* newpassword) {
    guard;
    if (m_curSelRoleIndex >= 0 && m_curSelRoleIndex < UILayerSelectChar::MaxUserCharBtn) {
        ChangeCharPassword(GetAccountInfo().users[m_curSelRoleIndex].id, old, newpassword);
    }
    unguard;
}

void GcLogin::OnDelCharPassword(const char* password) {
    guard;
    if (m_curSelRoleIndex >= 0 && m_curSelRoleIndex < UILayerSelectChar::MaxUserCharBtn) {
        DelCharPassword(GetAccountInfo().users[m_curSelRoleIndex].id, password);
    }
    unguard;
}

void GcLogin::OnCharPasswordConfirm(const char* password) {
    guard;
    if (m_curSelRoleIndex >= 0 && m_curSelRoleIndex < UILayerSelectChar::MaxUserCharBtn) {
        CharPasswordConfirm(GetAccountInfo().users[m_curSelRoleIndex].id, password);
    }
    unguard;
}

void GcLogin::OnNetDeleteUser(long id, char hasDel) {
    if (hasDel) {
        auto it = m_selRole_IDMapGcActor.find(id);
        if (it != m_selRole_IDMapGcActor.end()) {
            DEL_ONE(it->second);               // ɾ�������Դ
            m_selRole_IDMapGcActor.erase(it);  //�� map ��ɾ����Ԫ��
        }
    }
    QueryAccountInfo();  //���»�ȡ�û���Ϣ
}

void GcLogin::OnRandomCreateUser() {}

void GcLogin::OnCreateUser() {
    guard;
    CHECK(m_eCurrentStatus == GLS_CREATE_CHAR);
    if (m_curCrtRoleCsvID == -1) {
        ShowMessage(R(LMSG_PLS_CHOOSE_CHAR));
        return;
    }
    if (g_layerSelectChar->m_usrRoleName->GetText().length() < 4 ||
        g_layerSelectChar->m_usrRoleName->GetText().length() > 14) {
        ShowMessage(R(MSG_USERNAME_INVALID));
        return;
    }
    std::string  tmp         = g_layerSelectChar->m_usrRoleName->GetText();
    const char*  szUsername  = tmp.c_str();
    const char   m_bSex      = m_curCrtRoleCsvID == FengWu ? 1 : 0;
    const short& headModelID = m_crtRole_csvIdMapHeads[m_curCrtRoleCsvID][0].id;
    this->CreateChar(szUsername, m_curCrtRoleCsvID, m_selectFaction, m_bSex, headModelID,
                     m_strCharPassword.c_str());
    unguard;
}

//--------------------------------��Ⱦ �� �߼�֡--------------------------------
//lyymark 2.GcLogin.OnRun GcLogin֡��ѭ�����
void GcLogin::OnRun(float fSecond) {
    //lyytodo ��֡ ��Ϊ����graph��ѭ���������ͳһ�趨
    //CLockFrame lockFrame(1000.0 / 60);
    static RtgVertex3  highColor(1.0, 1.0, 1.0);
    static RtgVertex3  normalColor(0.0, 0.0, 0.0);
    int                i = 0, j = 0;
    POINT              cursorPt;
    const RtgMatrix12* oldWorldMatrix;
    RtgMatrix12        newWorldMatrix;
    //lyymark 2.GcLogin ��������
    g_pSoundMgr->UpdateAll(NULL, GetDevice()->GetAppTime());
    //���³������
    for (auto& pair : m_mapActor) {
        auto& act = pair.second;
        if (act) {
            act->Tick(fSecond * 1000.f);
        }
    }
    if (m_eCurrentStatus == GLS_SELECT_GAMEWORLD_SERVER) {  //�ж�����
        OnGuideNetProcess(fSecond);                         // �򵼵�����
    } else {
        this->Process();  // ��½������
    }
    if (m_eCurrentStatus == GLS_SELECT_CHAR || m_eCurrentStatus == GLS_CREATE_CHAR) {
        if (m_selRole_IDMapGcActor.size() != 0) {
            for (auto& [ID, GcActor] : m_selRole_IDMapGcActor) {
                if (!GcActor)
                    continue;
                if (!GcActor->GetGraph()->p()->IsPlayingPose()) {
                    GcActor->mBaseActor.PlayPose(GcBaseActor::POSE_STAND);
                }
                GcActor->Run(fSecond);
            }
        }
        if (m_crtRole_csvIdMapActIns.size() != 0) {
            if (!m_pSsJianPath->IsPlayingPose()) {
                m_pSsJianPath->PlayPose("wait_j0", false, 0.5f);
            }
            if (!m_pDsLunPath->IsPlayingPose()) {
                m_pDsLunPath->PlayPose("waiting_l1", false, 0.5f);
            }

            for (auto& [csvID, actIns] : m_crtRole_csvIdMapActIns) {
                if (!actIns)
                    continue;
                oldWorldMatrix = actIns->GetWorldMatrix();
                newWorldMatrix.SetRow(3, *oldWorldMatrix->Row(3));
                actIns->SetWorldMatrix(newWorldMatrix);
                if (!actIns->IsPlayingPose()) {
                    if (csvID == ZhanShi) {
                        actIns->PlayPose("wait_t_sin");
                    } else {
                        actIns->PlayPose("wait_login");
                    }
                }
                actIns->Tick(fSecond * 1000.f);  //��Ҫ
            }
        }
    }
    //UI��Ⱦ
    g_workspace.Run(fSecond * 1000);
    //�����������
    if (m_eCurrentStatus != GLS_SELECT_CHAR && m_eCurrentStatus != GLS_CREATE_CHAR)
        return;
    static float fDiffSecond = 0.f;
    fDiffSecond += fSecond;
    if (fDiffSecond < 0.2f)  //�������ˢ��ռ��cpu
        return;
    fDiffSecond = 0.0f;
    if ((g_workspace.IsMouseMonopolist() || !m_bCanInput))
        return;
    GetCursorPos(&cursorPt);
    ScreenToClient((HWND)GetApp()->GetWndHandle(), &cursorPt);
    if (cursorPt.x > 0 && cursorPt.x < GetDevice()->m_iWndWidth && cursorPt.y > 0 &&
        cursorPt.y < GetDevice()->m_iWndHeight && GetDevice()->HasFocus()) {
        if (m_selRole_IDMapGcActor.size()) {
            for (auto& [ID, GcActor] : m_selRole_IDMapGcActor) {
                const auto& actGraph = GcActor->GetGraph();
                if (GcActor == nullptr)
                    continue;
                if (DetectIntersection(cursorPt.x, cursorPt.y, actGraph->p()) ||
                    m_curSelRoleIndex == m_selRole_IDMapIndex[ID]) {
                    actGraph->SetBaseColor(highColor);
                } else
                    actGraph->SetBaseColor(normalColor);
            }
        }
        if (m_crtRole_csvIdMapActIns.size()) {
            for (auto& [csvID, actIns] : m_crtRole_csvIdMapActIns) {
                if (actIns == nullptr)
                    continue;
                if (DetectIntersection(cursorPt.x, cursorPt.y, actIns) ||
                    m_curCrtRoleCsvID == csvID) {
                    actIns->SetBaseColor(highColor);
                } else
                    actIns->SetBaseColor(normalColor);
            }
        }
    }
}

void GcLogin::OnBeginRender() {
    guard;
    unguard;
}

void GcLogin::OnEndRender() {}

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
            OnRenderLoading();
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

// -----------------------------��������Ӧ�ص�------------------------------
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
    vMin    = pAABB->Min();
    vMax    = pAABB->Max();
    // �ж�������AABB�Ƿ��ཻ
    return rtgIsLineIntersectAABB(v0, v1, vMin, vMax, &r1, &r2);
}

void GcLogin::OnRenderMask(RTGRenderMask mask, float fSecond) {
    guard;
    CRT_ActorInstance* pActor;
    RtgCamera*         pCamera = GetDevice()->m_pCamera;
    for (const auto& pair : m_mapActor) {
        pActor = pair.second;
        // �Ż���Ⱦ�߼� ��� Actor �Ƿ�ɼ�
        if (pCamera && !pCamera->CheckVisable(pActor->GetStaticBoundingBox()))
            continue;
        // �ж��Ƿ���Ҫ������Ч
        bool bFogChange = false;
        if (!m_lSkyFog && pair.first == "Sky") {
            bFogChange = true;
        }
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
        for (const auto& [id, actor] : m_selRole_IDMapGcActor) {
            if (actor)
                actor->Render(*GetDevice(), mask);
        }
    } else if (m_eCurrentStatus == GLS_CREATE_CHAR) {
        for (const auto& [csvId, actorIns] : m_crtRole_csvIdMapActIns) {
            if (actorIns)
                actorIns->Render(GetDevice(), mask, true, true);
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

static inline void moveToIndex(std::vector<int>& vec, int targetElement, int targetIndex) {
    auto it = std::find(vec.begin(), vec.end(), targetElement);
    if (it == vec.end()) {
        return;
    }
    int currentIndex = std::distance(vec.begin(), it);
    if (currentIndex == targetIndex) {
        return;
    }
    int              steps = (targetIndex - currentIndex + vec.size()) % vec.size();
    std::vector<int> temp(vec.size());
    for (int i = 0; i < vec.size(); ++i) {
        int newPos   = (i + steps) % vec.size();
        temp[newPos] = vec[i];
    }
    vec = std::move(temp);
}

static inline void moveToLeft(std::vector<int>& vec) {
    if (!vec.empty()) {
        std::rotate(vec.begin(), vec.begin() + 1, vec.end());
    }
}

static inline void moveToRight(std::vector<int>& vec) {
    if (!vec.empty()) {
        std::rotate(vec.rbegin(), vec.rbegin() + 1, vec.rend());
    }
}

static int turnFlag = 0;  //����״̬ ��ɾ

void GcLogin::OnMouseLDown(int iButton, int x, int y) {
    guard;
    if (!m_bCanInput || g_workspace.IsMouseMonopolist())
        return;
    switch (m_eCurrentStatus) {
        case GLS_SELECT_CHAR:
            if (!m_selRole_IDMapGcActor.size())
                break;
            for (auto& [ID, GcActor] : m_selRole_IDMapGcActor) {
                const auto& actGraph = GcActor->GetGraph();
                if (GcActor == nullptr)
                    continue;
                if (DetectIntersection(x, y, actGraph->p())) {
                    m_curSelRoleIndex = m_selRole_IDMapIndex[ID];
                    UpdateSelectChar();
                    break;
                }
            }
            break;
        case GLS_CREATE_CHAR:
            if (!m_crtRole_csvIdMapActIns.size())
                break;
            for (auto& [csvID, actIns] : m_crtRole_csvIdMapActIns) {
                if (actIns == nullptr)
                    continue;
                if (DetectIntersection(x, y, actIns)) {
                    m_curCrtRoleCsvID     = csvID;
                    const SRT_Pose* _pose = &actIns->GetCurrentPose();
                    if (_pose && _pose->Name != "waiting_login") {
                        actIns->PlayPose("waiting_login");
                    }
                    std::string& soltName = actIns->m_parentSlot;
                    if (turnFlag != 1 && soltName == "bno3") {
                        m_pBody->PlayPose("right");
                        turnFlag = 1;
                    }
                    if (soltName == "bno2") {
                        if (turnFlag == 1) {
                            m_pBody->PlayPose("right");
                            moveToRight(actSoltInTray);
                        } else if (turnFlag == 0) {
                            m_pBody->PlayPose("left");
                            moveToLeft(actSoltInTray);
                        }
                    }
                    if (turnFlag == 1 && soltName == "bno1") {
                        m_pBody->PlayPose("left");
                        turnFlag = 0;
                    }
                    UpdateCreateChar();
                    break;
                }
            }
            break;
    }
    unguard;
}

void GcLogin::OnMouseLDClick(int iButton, int x, int y) {
    if (!m_bCanInput)
        return;
}

void GcLogin::OnMouseRDown(int iButton, int x, int y) {
    if (!m_bCanInput)
        return;
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

//----------------------�������-----------------------
//��¼
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

/*----------------------------------------------------------------------------
-   Guide Net
----------------------------------------------------------------------------*/

class GcsGuideSession : public CG_TCPSession {
   public:
    GcsGuideSession(const char* szIP, int iPort) {
        m_szIP           = szIP;
        m_iPort          = iPort;
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
    char* pIP   = NULL;
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
            ms_pGameWorldServerList[i].lPort       = sPort;
            ms_pGameWorldServerList[i].szName      = pName;
            ms_pGameWorldServerList[i].szIP        = pIP;
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

// ��������Ҫ��д
int GcLogin::GetPing(char* zIP) {
    return 1;
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
                szEvaluation = "ά��...";
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
        g_layerLogin->mp_layerServer->OnInsertNewServer(tempConunt, szName, gwServer.ping,
                                                        szEvaluation);
        tempConunt++;
    }
    unguard;
}

void GcLogin::OnPoseBegin(SRT_Pose* pose) {}

void GcLogin::OnPoseEnd(SRT_Pose* pose) {}

void GcLogin::OnPoseEvent(SRT_Pose* pose, SRT_PoseEvent* event) {}

void GcLogin::OnPoseBreak(SRT_Pose* oldPose, SRT_Pose* newPose) {}

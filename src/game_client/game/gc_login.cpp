#include "gc_include.h"
#include "gc_cursor.h"
#include "gc_login.h"
#include "region_client.h"
#include "gc_login_session.h"
//#include "game_script_interface.h"
//#include "gc_constant.h"
#include "gs_protocol.h"
#include "lgs_gws.h"
//#include "trump_show.h"
#include "ui_form_msg.h"
#include "ui_form_textMsg.h"

//1--սʿ 4--���� 7--��ʿ 10--��ʿ

static enum CharacterType {
    ZhanShi = 1,  // սʿ
    FengWu = 4,   // ����
    ShuShi = 7,   // ��ʿ
    DaoShi = 10   // ��ʿ
};

static int s_userID[] = {ZhanShi, FengWu, ShuShi, DaoShi};

//static char* s_pszLoginCharName[] = {"role_wt_m_01", "role_hj_f_01", "role_ss_m_01",
//                                     "role_mj_f_01"};
//pf01 zs  ph01 fw    pn01 ss   pt01 ds
static char* s_pszLoginCharName[] = {"pf", "ph", "pn", "pt"};
static char* s_pszCharCreateBody[] = {"bpf011", "bph001", "bpn011", "bpt001"};
static char* s_pszCharCreateHand[] = {"hpf011", "hph001", "hpn011", "hpt001"};
static char* s_pszCharCreateLeg[] = {"lpf011", "lph001", "lpn011", "lpt001"};
//    std::rotate(arr.begin(), arr.begin() + index, arr.end());

static char* s_pszCharAnimalName[] = {
    "ui/textures/animal1.tga",  "ui/textures/animal2.tga",  "ui/textures/animal3.tga",
    "ui/textures/animal4.tga",  "ui/textures/animal5.tga",  "ui/textures/animal6.tga",
    "ui/textures/animal7.tga",  "ui/textures/animal8.tga",  "ui/textures/animal9.tga",
    "ui/textures/animal10.tga", "ui/textures/animal11.tga", "ui/textures/animal12.tga"};

std::string GcLogin::m_szAccountUsername = "";
std::string GcLogin::m_szAccountPassword = "";

GcLogin::GcLogin(CGameClientFrame* pGameClientFrame) : m_ini(true) {
    m_eStatus = GLS_NONE;
    m_eNextStatus = GLS_NONE;
    m_pGameClientFrame = pGameClientFrame;
    m_bSelectCharDone = false;
    m_bCanInput = false;
    m_iCurSelectChar = -1;
    m_bSex = SEX_MALE;
    m_pBody = nullptr;
    m_pCamera = nullptr;

    m_pZsDao = nullptr;
    m_pZsDao2 = nullptr;
    m_pSsJian = nullptr;
    m_pSsJianPath = nullptr;
    m_pMmGong = nullptr;
    m_pDsLun = nullptr;

    m_hallName.clear();
    m_hallKey.clear();
    m_ishallLogin = false;
    m_bForceLogin = false;
    iAnimalIndex = 0;
    headModelID = 0;
    headImageID = 0;
    bRandom = false;
    iRandomAnimalIndex = 0;
    headRandomModelID = 0;
    headRandomImageID = 0;
    m_ePrevHeadID = 0;
    m_eNextHeadID = 0;
    m_iLastServer = -1;
    bSelectUserWithPwd = false;
    m_nDisconnectCause = 0;
    bSaveAccount = false;
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
    ERR1("%s\n", pMsg);
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
void LoadLoginSection(RtIni* pIni, const std::string& szSectionName,
                      std::map<std::string, CRT_ActorInstance*>& mapActor) {
    guard;
    RtString           szLink, szName;
    CRT_ActorInstance *pBody, *pActor;
    pBody = nullptr;
    // ����INI�ļ���ָ�����ƵĽ�
    pIni->FindSection(szSectionName.c_str());
    // ��ȡ��һ����Ŀ
    if (pIni->FirstEntry(&szLink, &szName)) {
        do {
            // ����һ��actorʵ��
            pActor = RtcGetActorManager()->CreateActor(szName, true);
            if (pActor) {
                // ���actor���Ʋ���"role"��ͷ���򲥷Ŷ���
                if (szSectionName != "CreateChar") {
                    pActor->PlayPose(true);
                }
                // ��actorʵ����ӵ�ӳ����
                mapActor[std::string(szLink)] = pActor;
                // �����������Ϊ"Body"���򽫵�ǰactorʵ����Ϊ����
                if (szLink == "Body") {
                    pBody = pActor;
                } else {
                    // ֱ�����ӷ� "Body" �� actor ʵ��
                    if (pBody)  // ȷ�� pBody �ѱ�����
                    {
                        pActor->LinkParent(pBody, szLink.c_str());
                    }
                }
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
    guard;
    {
        std::map<std::string, CRT_ActorInstance*>::iterator it;
        for (it = m_mapLogin.begin(); it != m_mapLogin.end(); it++) {
            RtcGetActorManager()->ReleaseActor((*it).second);
        }
        for (it = m_mapSelectChar.begin(); it != m_mapSelectChar.end(); it++) {
            RtcGetActorManager()->ReleaseActor((*it).second);
        }
        for (it = m_mapCreateChar.begin(); it != m_mapCreateChar.end(); it++) {
            if ((*it).first == "s_dahuangdi") {
                int a = 1;
            }
            RtcGetActorManager()->ReleaseActor((*it).second);
        }
    }

    {
        std::map<DWORD, GcActor*>::iterator itActor;
        for (itActor = m_mapSelectActor.begin(); itActor != m_mapSelectActor.end(); itActor++) {
            DEL_ONE((*itActor).second);
        }
    }

    RtcGetActorManager()->ReleaseActor(m_pZsDao);
    RtcGetActorManager()->ReleaseActor(m_pZsDao2);
    RtcGetActorManager()->ReleaseActor(m_pSsJian);
    RtcGetActorManager()->ReleaseActor(m_pSsJianPath);
    RtcGetActorManager()->ReleaseActor(m_pMmGong);
    RtcGetActorManager()->ReleaseActor(m_pDsLun);

    GetDevice()->m_dwClearColor = 0x00000000;

    UIFormTextMsg::Clear();
    UIFormMsg::Clear(false);

    return true;
    unguard;
}

//lyymark 2.GcLogin ��¼״̬��
void GcLogin::SetLoginState(EStatus eState) {
    guard;
    m_ePrevStatus = m_eStatus;
    m_eNextStatus = eState;

    // �뿪
    bool bLeaveNow = true;
    switch (m_eStatus) {
        case GLS_LOADING:
            LeaveLoading();
            break;
        case GLS_SELECT_GAMEWORLD_SERVER:
            bLeaveNow = LeaveSelectGameWorldServer();
            break;
        case GLS_LOGIN:
            bLeaveNow = LeaveLogin();
            break;
        case GLS_SELECT_CHAR:
            bLeaveNow = LeaveSelectChar();
            break;
        case GLS_CREATE_CHAR:
            bLeaveNow = LeaveCreateChar();
            break;
    }

    // ����
    if (!bLeaveNow) {
        return;
    } else {
        m_eNextStatus = GLS_NONE;
        m_eStatus = eState;
    }

    // ����
    switch (m_eStatus) {
        case GLS_LOADING:
            EnterLoading();
            break;
        case GLS_SELECT_GAMEWORLD_SERVER:
            m_mapActor = m_mapLogin;
            Lyy_UpdateCameraPos();
            UpdateGraphConfig("Graph_Login");
            EnterSelectGameWorldServer();
            break;
        case GLS_LOGIN:
            m_mapActor = m_mapLogin;
            Lyy_UpdateCameraPos();
            UpdateGraphConfig("Graph_Login");
            EnterLogin();
            break;
        case GLS_SELECT_CHAR:
            m_mapActor = m_mapSelectChar;
            Lyy_UpdateCameraPos();
            UpdateGraphConfig("Graph_SelectChar");
            EnterSelectChar();
            break;
        case GLS_CREATE_CHAR:
            m_mapActor = m_mapCreateChar;
            Lyy_UpdateCameraPos();
            UpdateGraphConfig("Graph_CreateChar");
            EnterCreateChar();
            break;
    }
    unguard;
}

CRT_ActorInstance* GcLogin::FindModel(const std::string& szName) {
    guard;
    auto it = m_mapActor.find(std::string(szName));
    if (it != m_mapActor.end())
        return it->second;
    return NULL;
    unguard;
}

namespace {

/**
	����ģ�͹��������� 4x3 ����RtgMatrix12����
	����������� 4x4 ����RtgMatrix16����
	��Ҫһ��ת������ _AdjustCameraMatrix��
	*/
inline void _AdjustCameraMatrix(RtgMatrix16* matOut, RtgMatrix12* matIn) {
    RTASSERT(matOut);
    RTASSERT(matIn);
    RtgVertex3 z = *(RtgVertex3*)&matIn->_10;
    z.Normalize();
    z.Negative();
    RtgVertex3 x = RtgVertex3(0.f, 0.f, 1.f).Cross(z);
    x.Normalize();
    RtgVertex3 y = z.Cross(x);
    y.Normalize();
    //����Ϊ��λ����
    matOut->Identity();
    matOut->SetRow(0, x);
    matOut->SetRow(1, y);
    matOut->SetRow(2, z);
    matOut->SetRow(3, (float*)&matIn->_30);
}

inline void Lyy_AdjustCameraMatrix(RtgMatrix16* matOut, RtgMatrix12* matIn) {
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

// �������λ��
void GcLogin::UpdateCameraPos() {
    RtgMatrix16 mCamera;

    CRT_ActorInstance* pCamera;
    m_pBody = FindModel("Body");
    pCamera = m_pBody;

    if (pCamera) {
        RtgMatrix12 _mat;
        if (m_pBody->GetBoneMat(&_mat, "bcam")) {
            _AdjustCameraMatrix(&mCamera, &_mat);
            GetDevice()->m_pCamera->SetMatrix(mCamera);
        }
    }
}

// lyymark 2.GcLogin �޸����λ�þ��� by lyy 2024-07-30
void GcLogin::Lyy_UpdateCameraPos() {
    RtgMatrix16        mCamera;
    CRT_ActorInstance* pCamera;
    m_pBody = FindModel("Body");
    pCamera = m_pBody;
    if (pCamera) {
        RtgMatrix12 _mat;
        if (m_pBody->GetBoneMat(&_mat, "bcam") || m_pBody->GetBoneMat(&_mat, "Bcam")) {
            Lyy_AdjustCameraMatrix(&mCamera, &_mat);
            GetDevice()->m_pCamera->SetMatrix(mCamera);
            /*  P_LOGINFO("BodyMatrix:\n" + _mat.ToString());
            P_LOGINFO("CameraMatrix:\n" + mCamera.ToString());*/
        }
    }
}

void GcLogin::EnterSelectGameWorldServer() {
    DXUtil_Timer(TIMER_RESET);  //���ڽ��֡�����ƿ��ټ���
    if (m_pCamera) {
        m_pCamera = nullptr;
    }

    if (g_layerLogin == nullptr) {
        //lyymark 2.GcLogin.UI ���ص�¼�ͷ������б�UI
        UILayer::EnterLogin();
    }
    StartGetGameWorldServer();
    //��ʾ �������б�
    g_layerLogin->mp_layerServer->Show();
}

bool GcLogin::LeaveSelectGameWorldServer() {
    CHECK(m_eStatus == GLS_SELECT_GAMEWORLD_SERVER);
    g_layerLogin->mp_layerServer->Hide();
    EndGetGameWorldServer();
    return true;
}

void GcLogin::readAccountFromFile() {
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
    if (m_pCamera) {
        m_pCamera = nullptr;
    }
    if (g_layerLogin == nullptr) {
        UILayer::EnterLogin();
    }
    readAccountFromFile();
    g_layerLogin->Show();
}

bool GcLogin::LeaveLogin() {
    guard;
    CHECK(m_eStatus == GLS_LOGIN);
    g_layerLogin->Hide();
    return true;
    unguard;
}

void GcLogin::EnterSelectChar() {
    guard;
    DXUtil_Timer(TIMER_RESET);  //���ڽ��֡�����ƿ��ټ���
    if (m_pCamera) {
        m_pCamera = NULL;
    }
    if (g_layerSelectChar == nullptr) {
        UILayer::EnterSelectChar();
    }

    LOAD_UI("Pnbutton")->Show();
    LOAD_UI("btnenter")->Show();
    LOAD_UI("btndeletechar")->Show();
    LOAD_UI("btnsetcharpwd")->Show();
    LOAD_UI("btnsetcharpwd")->Disable();  //��ֹ���ý�ɫ���� ��Ϊ����д��  lyy  2024.8.2
    LOAD_UI("btnRMB")->Show();
    LOAD_UI("btnRMB")->Disable();  //��ֹ��ֵ���� ��Ϊ����д��  lyy  2024.8.2
    LOAD_UI("RaleInfo")->Hide();
    UpdateSelectChar();
    m_iCurSelectChar = -1;
    m_bCanInput = true;
    unguard;
}

void GcLogin::UpdateSelectChar() {
    guard;
    // ��鵱ǰ״̬�Ƿ�Ϊѡ���ɫ״̬�������򷵻�
    if (m_eStatus != GLS_SELECT_CHAR)
        return;

    m_selectUserCharActs.clear();
    m_selectUserCharIds.clear();
    //��ȡ�˺���Ϣ
    const GcAccountInfo& accountInfo = GetAccountInfo();

    if (accountInfo.chatCount == 0 || m_iCurSelectChar == -1) {  //�����ɫ����Ϊ0��û��ѡ�н�ɫ
        LOAD_UI("btnenter")->Disable();                          //��ֹ������Ϸ
        LOAD_UI("btndeletechar")->Disable();                     //��ֹɾ����ɫ
    }
    // ��ȡ��ť���󣬳�ʼ��δѡ��
    for (auto& btn : g_layerSelectChar->m_charBtnArray) {
        btn->Disable();
        btn->SetText("��");
        btn->SetTextColor(0xffffffff);  //��ɫ
    }
    // ���ݽ�ɫ����������Ӧ�İ�ť
    for (std::size_t i = 0; i < accountInfo.chatCount && i < g_layerSelectChar->MaxUserCharBtn;
         i++) {
        g_layerSelectChar->m_charBtnArray[i]->Enable();
    }

    GcActor*          pActor = nullptr;
    const SUserActor* pUserActorInCsv = nullptr;
    const SCreModel*  pActorModelInCsv = nullptr;
    // �����û���ɫ�б�,������Ӧģ�ͣ��Ѿ������Ļ�����
    for (std::size_t i = 0; i < accountInfo.chatCount; i++) {
        auto& user = accountInfo.users[i];
        if (m_mapSelectActor.find(user.id) != m_mapSelectActor.end())
            continue;
        // �����û���Ϣ�������ɫactorID
        // �� user_actor.csv����ҽ�ɫ��Ϣ,������Ӧģ��id��
        pUserActorInCsv = g_TableUserActor.FindUserActor(user.attributes.actorID);
        if (!pUserActorInCsv)
            continue;
        pActorModelInCsv = g_TableCreModel.FindCreModel(pUserActorInCsv->ModelId);
        if (!pActorModelInCsv)
            continue;
        // �����½�ɫ����
        pActor = RT_NEW GcActor;
        pActor->CreateGraphData((SCreModel*)pActorModelInCsv, user.headModelID);  //�ϳɽ�ɫģ��
        pActor->SetName(user.name);  //�û����ݿ���еĽ�ɫ��
        pActor->DisplayActor(true);
        // ���ؽ�ɫ��HUD��ͷ����ʾ��Ϣ��
        if (pActor->m_pHUD) {
            pActor->m_pHUD->Hide();
        }
        // ���ý�ɫ��ְҵ���Ա�����
        pActor->m_core.Metier = user.attributes.metier;
        pActor->m_core.Sex = user.attributes.Sex;
        // ���ؽ�ɫ����Ʒ��Ϣ
        CItemManager* pItemMgr = ((CGameClientFrame*)GetApp())->m_pItemManager;
        if (pItemMgr) {
            SItemID item;
            if (pActor->mItem.LoadFromString(user.items) <= 0) {
                // �����Ʒ��Ϣ��Ч�����ص�¼״̬����ʾ������Ϣ
                SetLoginState(GLS_LOGIN);
                ShowMessage(R(LMSG_USERDATA_INVALID));
                return;
            }
        }
        if (pActor)
            m_mapSelectActor[user.id] = pActor;
    }
    //����ģ���б��ȡ��ѡ��ɫ�����������
    for (std::size_t i = 0; i < accountInfo.chatCount; i++) {
        if (m_mapSelectActor.find(accountInfo.users[i].id) != m_mapSelectActor.end()) {
            pActor = m_mapSelectActor[accountInfo.users[i].id];
        }
        m_selectUserCharIds.push_back(accountInfo.users[i].id);
        m_selectUserCharActs.push_back(pActor);
    }

    if (accountInfo.chatCount < 3)
        LOAD_UI("btncreaterole")->Enable();
    else
        LOAD_UI("btncreaterole")->Disable();

    if (m_pBody) {
        RtgMatrix12 _SlotMatrix;
        std::size_t nSize = std::min(m_selectUserCharActs.size(), static_cast<std::size_t>(3));
        for (std::size_t i = 0; i < nSize; ++i) {
            auto&       user = accountInfo.users[i];
            std::string boneName = "bno" + std::to_string(i + 1);
            if (m_pBody->GetBoneMat(&_SlotMatrix, boneName.c_str())) {
                SUserActor* pActor =
                    g_TableUserActor.FindUserActor(user.attributes.metier,        //ְҵ
                                                   user.attributes.metierLevel);  //��ת��
                if (!pActor)
                    continue;
                std::string RaleName = user.name;
                g_layerSelectChar->m_charBtnArray[i]->SetText(RaleName);
                m_selectUserCharActs[i]->mBaseActor.SetMatrix_Row(3, *_SlotMatrix.Row(3));

                if (i == m_iCurSelectChar) {
                    g_layerSelectChar->m_charBtnArray[m_iCurSelectChar]->SetTextColor(
                        0xffff0000);  //��ɫ
                    LOAD_UI("btnenter")->Enable();
                    LOAD_UI("btndeletechar")->Enable();
                    std::string Lev = "Lv: " + std::to_string(user.attributes.level);
                    std::string Faction = g_layerSelectChar->m_faction[user.attributes.faction];
                    std::string Miter = pActor->MetierName;
                    LOAD_UI("RaleInfo")->Show();
                    LOAD_UI("RaleInfo.RaleName")->SetText(RaleName);
                    LOAD_UI("RaleInfo.Lev")->SetText(Lev);
                    LOAD_UI("RaleInfo.Faction")->SetText(Faction);
                    LOAD_UI("RaleInfo.Miter")->SetText(Miter);
                    P_LOGINFO("��ǰѡ��Ľ�ɫidΪ:" + std::to_string(user.id));
                }
            }
        }
    }
    for (std::size_t i = 0; i < m_selectUserCharActs.size(); i++) {
        pActor = m_selectUserCharActs[i];
        if (pActor && pActor->m_pHUD) {
            pActor->SetName(accountInfo.users[i].name);
            //pActor->m_pHUD->Hide();
            pActor->Run(0.01f);
        }
    }
    // OnRun(0.f);
    unguard;
}

bool GcLogin::LeaveSelectChar() {
    guard;
    CHECK(m_eStatus == GLS_SELECT_CHAR);

    m_bCanInput = false;

    LOAD_UI("Pnbutton")->Hide();
    LOAD_UI("btnenter")->Hide();
    LOAD_UI("btndeletechar")->Hide();
    LOAD_UI("btnsetcharpwd")->Hide();
    LOAD_UI("btnRMB")->Hide();
    LOAD_UI("RaleInfo")->Hide();
    //������ת��ť
    //LOAD_UI("btnuserleft")->Hide();
    //LOAD_UI("btnuserright")->Hide();

    m_selectUserCharActs.clear();
    m_selectUserCharIds.clear();
    return true;
    unguard;
}

void GcLogin::SetSelectUser(int iSel) {
    if (iSel == m_iCurSelectChar) {
        return;
    }
    switch (m_eStatus) {
        case GLS_SELECT_CHAR: {
            m_iCurSelectChar = iSel;
            UpdateSelectChar();
        } break;
        case GLS_CREATE_CHAR: {
            m_iCurSelectChar = iSel;
            vector<SHeadModel> heads;
            if (!g_TableHeadModel.GetHeadModel(s_userID[m_iCurSelectChar], heads))
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
    if (iSei == -1) {
        m_selectFaction = -1;
        if (m_pCamera)
            m_pCamera = nullptr;
        if (m_pCamera = FindModel("Door")) {
            m_pCamera->PlayPose("close");
            m_pCamera->RegisterNotify(nullptr);
        }
        LOAD_UI("fmorder")->Show();
        LOAD_UI("fmcreatid")->Hide();
        return;
    }

    if (iSei != 1 && iSei != 2)
        return;

    switch (iSei) {
        case 1: {
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
        case 2: {
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
    if (m_pCamera)
        m_pCamera = nullptr;
    if (m_pCamera = FindModel("Door")) {
        m_pCamera->PlayPose("open");
        m_pCamera->RegisterNotify(nullptr);
    }
    m_selectFaction = iSei;
    LOAD_UI("fmorder")->Hide();
    LOAD_UI("fmcreatid")->Show();
}

//lyymark 3.Gclogin �����ɫ����
void GcLogin::EnterCreateChar() {
    guard;
    //����
    if (m_pCamera)
        m_pCamera = nullptr;
    if (m_pCamera = FindModel("Door")) {
        m_pCamera->RealUseFrame(13);  //��ʼ���ŵ�״̬Ϊ��13֡  �ر�״̬
        m_pCamera->RegisterNotify(nullptr);
    }
    m_iCurSelectChar = -1;

    // ���ɿ�ѡ������б�
    int i, iUserIdx;
    iUserIdx = 0;
    const SUserActor*                                   p_userActCsv;
    CRT_ActorInstance*                                  pActor;
    std::map<std::string, CRT_ActorInstance*>::iterator it;
    // ��� m_mapCreateActor Ϊ�գ���ζ��û�д����κν�ɫģ��
    if (m_mapCreateActor.size() == 0) {
        // �����ɴ�����ɫ user_actor.csv
        for (i = 0; i < g_TableUserActor.GetUserCount(); i++) {
            //��ȡ����һ��
            p_userActCsv = g_TableUserActor.GetUserByIdx(i);
            if (p_userActCsv) {  // ����û��������
                if (!p_userActCsv->bZCreate &&
                    !p_userActCsv->bSCreate)  // ����û����ܶ����ܴ���������
                    continue;
                // ��� m_mapCreateActor �в����ڸ��û��� ID
                if (m_mapCreateActor.find(p_userActCsv->Id) == m_mapCreateActor.end()) {
                    // ��ȡ�ɴ�����ɫ������ 1��4��7��<16>0
                    // static int s_userID[] = {ZhanShi, FengWu, ShuShi/ DaoShi};

                    iUserIdx = GetCharIndexByActorID(p_userActCsv->Id);
                    if (iUserIdx < 0)
                        break;
                    pActor = FindModel(s_pszLoginCharName[iUserIdx]);
                    if (pActor) {
                        // Load Skin and Link Weapon
                        pActor->LoadSkin(s_pszCharCreateBody[iUserIdx], false);
                        pActor->LoadSkin(s_pszCharCreateHand[iUserIdx], false);
                        pActor->LoadSkin(s_pszCharCreateLeg[iUserIdx], false);

                        vector<SHeadModel> heads;
                        if (g_TableHeadModel.GetHeadModel(s_userID[iUserIdx], heads)) {
                            m_HeadModelIndex[iUserIdx] = 0;
                            pActor->LoadSkin(heads[0].skin.c_str(), true);
                        }
                        if (p_userActCsv->Id == ZhanShi)  //if (iUserIdx == 0)
                        {
                            //���� ZS����
                            if (!m_pZsDao && !m_pZsDao2) {
                                const std::string mWTModelName = "iw0401dao_04";  //"iwzs01_04";
                                m_pZsDao =
                                    RtcGetActorManager()->CreateActor(mWTModelName.c_str(), true);
                                m_pZsDao2 =
                                    RtcGetActorManager()->CreateActor(mWTModelName.c_str(), true);
                                if (m_pZsDao == NULL) {
                                    ERR1("[GcActor::Render] �޷���������ģ�ͣ�name=%s.\n",
                                         mWTModelName.c_str());
                                    P_LOGINFO("[GcActor::Render] �޷���������ģ��:" + mWTModelName);
                                    return;
                                }
                                m_pZsDao->LinkParent(pActor, "Box01");
                                m_pZsDao2->LinkParent(pActor, "Box02");
                            }
                        } else if (p_userActCsv->Id == FengWu)  //else if (iUserIdx == 1)
                        {
                            //���� MM ����
                            if (!m_pMmGong) {
                                const std::string mHJModelName = "iw0401gong_04";
                                m_pMmGong =
                                    RtcGetActorManager()->CreateActor(mHJModelName.c_str(), true);
                                if (m_pMmGong == NULL) {
                                    ERR1("[GcActor::Render] �޷���������ģ�ͣ�name=%s.\n",
                                         mHJModelName.c_str());
                                    P_LOGINFO("[GcActor::Render] �޷���������ģ��:" + mHJModelName);
                                    return;
                                }
                                m_pMmGong->LinkParent(pActor, "Box01");
                            }
                        } else if (p_userActCsv->Id == ShuShi)  //��ʿ
                        {
                            //������ɽ���ֺ͹켣����
                            if (!m_pSsJian) {
                                const std::string mSSModelName = "iw0501jian_04";
                                m_pSsJian =
                                    RtcGetActorManager()->CreateActor(mSSModelName.c_str(), true);
                                if (m_pSsJian == NULL) {
                                    ERR1("[GcActor::Render] �޷���������ģ�ͣ�name=%s.\n",
                                         mSSModelName.c_str());
                                    return;
                                }
                            }

                            if (!m_pSsJianPath) {
                                const std::string mWayName = "ss_fj";
                                m_pSsJianPath =
                                    RtcGetActorManager()->CreateActor(mWayName.c_str(), true);
                                if (m_pSsJianPath == NULL) {
                                    ERR1("[GcActor::Render] �޷���������ģ�ͣ�name=%s.\n",
                                         mWayName.c_str());
                                    return;
                                }
                            }
                        } else if (p_userActCsv->Id == DaoShi)  //��ʿ
                        {
                            if (!m_pDsLun) {
                                const std::string mMJModelName = "iw47";
                                m_pDsLun =
                                    RtcGetActorManager()->CreateActor(mMJModelName.c_str(), true);
                                if (m_pDsLun == NULL) {
                                    ERR1("[GcActor::Render] �޷���������ģ�ͣ�name=%s.\n",
                                         mMJModelName.c_str());
                                    P_LOGINFO("[GcActor::Render] �޷���������ģ��:" + mMJModelName);

                                    return;
                                }
                                m_pDsLun->LinkParent(pActor, "Box01");
                            }
                        }
                    }

                    m_mapCreateActor[p_userActCsv->Id] = pActor;
                }
            }
        }
    }

    //����ɾ��  ��ֹ���淴��lode skiƤ��
    for (i = 0; i < 4; i++) {
        it = m_mapActor.find(std::string(s_pszLoginCharName[i]));
        if (it != m_mapActor.end())
            m_mapActor.erase(it);
    }

    m_creatActorList.clear();
    m_selectUserCharIds.clear();

    for (i = 0; i < g_TableUserActor.GetUserCount(); i++) {
        p_userActCsv = g_TableUserActor.GetUserByIdx(i);
        if (p_userActCsv) {
            if (!p_userActCsv->bZCreate && !p_userActCsv->bSCreate)
                continue;
            if (m_mapCreateActor.find(p_userActCsv->Id) != m_mapCreateActor.end()) {
                pActor = m_mapCreateActor[p_userActCsv->Id];
            }
            m_selectUserCharIds.push_back(p_userActCsv->Id);
            m_creatActorList.push_back(pActor);
        }
    }
    if (m_selectUserCharIds.size() == 0) {
        ShowMessage(R(LMSG_CANNOT_CREATE_CHAR));
    }

    // Link ����---bno1����Ů-bno2��ɽ��
    m_pBody = FindModel("Body");
    if (m_pBody) {
        RtgMatrix16        mActorMatrix;
        RtgMatrix12        WeaponMatrix;
        RtgMatrix12        _SlotMatrix;
        const RtgMatrix12* oldWorldMatrix;
        RtgMatrix12        newWorldMatrix;
        // Modified by Wayne Wong 2010-11-26 for Demo
        for (size_t index = 0; index < m_selectUserCharIds.size(); ++index) {
            short actorID = m_selectUserCharIds[index];
            if (actorID == ZhanShi)  //սʿ
            {
                m_creatActorList[index]->LinkParent(m_pBody, "bno1");

            } else if (actorID == FengWu)  //����
            {
                m_creatActorList[index]->LinkParent(m_pBody, "bno2");
                oldWorldMatrix = m_creatActorList[index]->GetWorldMatrix();
                newWorldMatrix.SetRow(3, *oldWorldMatrix->Row(3));
                m_creatActorList[index]->SetWorldMatrix(newWorldMatrix);

            } else if (actorID == ShuShi)  //��ʿ
            {
                m_creatActorList[index]->LinkParent(m_pBody, "bno3");
                oldWorldMatrix = m_creatActorList[index]->GetWorldMatrix();
                newWorldMatrix.SetRow(3, *oldWorldMatrix->Row(3));
                m_creatActorList[index]->SetWorldMatrix(newWorldMatrix);

                // ��ȡ�����ľ���
                WeaponMatrix = *m_pSsJian->GetMatrix();
                // ��������������Ϊ��ɫ����
                WeaponMatrix = mActorMatrix;
                // ������������� Z ��λ�ã���ǰ�ƶ� 25 ��λ
                WeaponMatrix._32 += 25.f;
                // ��������·���ľ���Ϊ���������������
                m_pSsJianPath->SetMatrix(WeaponMatrix);
                // ������·����ѡ��Ľ�ɫ����
                m_pSsJianPath->LinkParent(m_creatActorList[index]);

            } else if (actorID == DaoShi)  //��ʿ
            {
                if (m_pBody->GetBoneMat(&_SlotMatrix, "root")) {
                    Lyy_AdjustCameraMatrix(&mActorMatrix, &_SlotMatrix);
                    mActorMatrix = _SlotMatrix;
                    mActorMatrix.RotateLX(DegreeToRadian(28.f));
                    mActorMatrix.RotateLY(DegreeToRadian(10.f));
                    mActorMatrix.RotateLZ(DegreeToRadian(-90.f));
                    m_creatActorList[index]->SetMatrix(mActorMatrix);
                }
            }
        }
    }

    OnEnterCreateChar();
    unguard;
}

bool GcLogin::LeaveCreateChar() {
    guard;
    CRT_ActorInstance* pActor;
    if (m_bCanInput) {
        CHECK(m_eStatus == GLS_CREATE_CHAR);
        if (m_iCurSelectChar < 0 || m_iCurSelectChar > 4) {
            pActor = NULL;
        }

        m_iCurSelectChar = -1;

        OnLeaveCreateChar();
    }

    m_creatActorList.clear();
    m_pCamera = NULL;
    g_pMusicThread->Stop();
    g_pMusicThread->Play("login_bg.ogg", true);
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

    m_pBody->RegisterNotify(this);
    OnRun(0.f);
}

void GcLogin::OnLeaveCreateChar() {
    m_pBody->RegisterNotify(NULL);
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
    m_selectUserCharActs[m_iCurSelectChar]->GetGraph()->p()->RotateLZ(DegreeToRadian(30.f));

    /*
	guard;
	RtgMatrix12* pActorMatrix;
	pActorMatrix = m_selectUserCharActs[m_iCurSelectChar]->GetGraph()->p()->GetMatrix();
	//RtgMatrix16* pActorMatrix;
	//pActorMatrix = m_selectUserCharActs[m_iCurSelectChar]->mBaseActor.Matrix();
	pActorMatrix->RotateLZ(DegreeToRadian(30.f));
	//m_selectUserCharActs[m_iCurSelectChar]->GetGraph()->SetRotation(*pActorMatrix);
	unguard;

	*/
}

static int mRotationTime = 0;

void GcLogin::OnLeftRotationDown() {
    mRotationTime++;
    m_selectUserCharActs[m_iCurSelectChar]->GetGraph()->p()->RotateLZ((mRotationTime * 30) % 360);

    /*
	RtgMatrix12* pActorMatrix;
	pActorMatrix = m_selectUserCharActs[m_iCurSelectChar]->GetGraph()->p()->GetMatrix();
	pActorMatrix->RotateLZ(DegreeToRadian((mRotationTime*30)%360));
	*/
}

void GcLogin::OnLeftRotationUp() {}

//�����ɫ����
void GcLogin::OnRightRotation() {
    m_selectUserCharActs[m_iCurSelectChar]->GetGraph()->p()->RotateLZ(DegreeToRadian(-30.f));

    /*
		guard;
		RtgMatrix12* pActorMatrix;
		pActorMatrix = m_selectUserCharActs[m_iCurSelectChar]->GetGraph()->p()->GetMatrix();
		//RtgMatrix16* pActorMatrix;
		//pActorMatrix = m_selectUserCharActs[m_iCurSelectChar]->mBaseActor.Matrix();
		//pActorMatrix->_22 -= 30.f;
		pActorMatrix->RotateLZ(DegreeToRadian(-30.f));
		//m_selectUserCharActs[m_iCurSelectChar]->GetGraph()->SetRotation(*pActorMatrix);
		unguard;
		*/
}

//������Ϸ
void GcLogin::OnSelectUser() {
    guard;
    if (m_iCurSelectChar >= 0 && m_iCurSelectChar < g_layerSelectChar->MaxUserCharBtn) {
        UILayer::EnterLoading();
        SelectChar(GetAccountInfo().users[m_iCurSelectChar].id);
    } else {
        ShowMessage(R(LMSG_PLS_CHOOSE_CHAR));
    }
    unguard;
}

void GcLogin::OnSelectUserWithPwd() {
    UILayer::EnterLoading();
    SelectChar(GetAccountInfo().users[m_iCurSelectChar].id);
}

void GcLogin::OnRestoreChar() {
    guard;
    if (m_iCurSelectChar >= 0 && m_iCurSelectChar <= 4) {
        RestoreChar(GetAccountInfo().users[m_iCurSelectChar].id);
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
    if (m_iCurSelectChar >= 0 && m_iCurSelectChar < 4) {
        DeleteChar(GetAccountInfo().users[m_iCurSelectChar].id);
    } else {
        ShowMessage(R(LMSG_PLS_CHOOSE_CHAR));
    }
    unguard;
}

//���ý�ɫ����
void GcLogin::OnSetCharPassword(const char* password) {
    guard;
    if (m_iCurSelectChar >= 0 && m_iCurSelectChar < 4) {
        SetCharPassword(GetAccountInfo().users[m_iCurSelectChar].id, password);
    }
    unguard;
}

//�����ɫ����
void GcLogin::OnChangeCharPassword(const char* old, const char* newpassword) {
    guard;
    if (m_iCurSelectChar >= 0 && m_iCurSelectChar < 4) {
        ChangeCharPassword(GetAccountInfo().users[m_iCurSelectChar].id, old, newpassword);
    }
    unguard;
}

//���������
void GcLogin::OnDelCharPassword(const char* password) {
    guard;
    if (m_iCurSelectChar >= 0 && m_iCurSelectChar < 4) {
        DelCharPassword(GetAccountInfo().users[m_iCurSelectChar].id, password);
    }
    unguard;
}

void GcLogin::OnCharPasswordConfirm(const char* password) {
    guard;
    if (m_iCurSelectChar >= 0 && m_iCurSelectChar < 4) {
        CharPasswordConfirm(GetAccountInfo().users[m_iCurSelectChar].id, password);
    }
    unguard;
}

void GcLogin::ResetButtonPos() {
    if (LOAD_UI("btnsetcharpwd")->IsVisible()) {
        LOAD_UI("btnsetcharpwd")->Hide();
    }
}

void GcLogin::OnNetDeleteUser(long id, char hasDel) {
    if (hasDel) {
        std::map<DWORD, GcActor*>::iterator it = m_mapSelectActor.find(id);
        if (it != m_mapSelectActor.end()) {
            DEL_ONE((*it).second);
            m_mapSelectActor.erase(it);
        }
    }
    QueryAccountInfo();
}

void GcLogin::OnRandomCreateUser() {
    guard;
    CHECK(m_eStatus == GLS_CREATE_CHAR);
    //���������־
    bRandom = true;

    int iCharIndex = GetCharIndexByActorID(m_selectUserCharIds[m_iCurSelectChar]);

    //��ʾ�������
    RtwImage* pImage;
    pImage = g_workspace.getImageFactory()->createImage(s_pszCharAnimalName[iRandomAnimalIndex]);
    pImage->SetBlend(true);
    LOAD_UI("fmcreatid1.fmanimal.lbanimal")->SetBackgroundImage(pImage);

    vector<SHeadModel> heads;
    if (!g_TableHeadModel.GetHeadModel(s_userID[iCharIndex], heads))
        return;

    int size = heads.size();
    m_eNextHeadID = rand() % (heads.size());
    if (m_ePrevHeadID != m_eNextHeadID) {
        m_creatActorList[m_iCurSelectChar]->UnloadSkin(heads[m_ePrevHeadID].skin.c_str());
        m_creatActorList[m_iCurSelectChar]->LoadSkin(heads[m_eNextHeadID].skin.c_str(), true);
        LOAD_UI("fmcreatid2.fmhair.lbhairname")->SetText(heads[m_eNextHeadID].name);
        //LOAD_UI("fmcreatid2.fmface.lbfacename")->SetText(heads[m_HeadModelIndex[iCharIndex]].name);
    }
    m_ePrevHeadID = m_eNextHeadID;

    vector<SCharImage> images;
    if (!g_TableCharImage.GetCharImage(s_userID[iCharIndex], images))
        return;
    headRandomImageID = rand() % 4;
    std::string strImage = "ui_texture/";
    strImage += images[headRandomImageID].imageCreate;
    strImage += ".dds";

    pImage = g_workspace.getImageFactory()->createImage(strImage);
    pImage->SetBlend(true);
    LOAD_UI("fmcreatid2.fmhead.lbpichead")->SetBackgroundImage(pImage);

    unguard;
}

void GcLogin::OnCreateUser() {
    guard;
    CHECK(m_eStatus == GLS_CREATE_CHAR);

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
    if (g_TableHeadModel.GetHeadModel(s_userID[iCharIndex], heads)) {
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

    guard;
    //lyytodo ��֡ ��Ϊ����graph��ѭ���������ͳһ�趨
    //CLockFrame lockFrame(1000.0 / 60);
    int                i, j;
    RtgVertex3         vOrig, vDir, v0, v1, vMin, vMax;
    float              r1, r2;
    RtgAABB*           pAABB;
    POINT              pt;
    RtgMatrix16        m16;
    static float       fDiffSecond = 0.f;
    CRT_ActorInstance* pActor;

    //lyymark 2.GcLogin ��������
    g_pSoundMgr->UpdateAll(NULL, GetDevice()->GetAppTime());

    for (auto& pair : m_mapActor) {
        auto& act = pair.second;
        if (act) {
            act->Tick(fSecond * 1000.f, false);
        }
    }

    if (m_pSsJian) {
        m_pSsJian->Tick(fSecond * 1000.f, false);
    }
    if (m_pSsJianPath) {
        m_pSsJianPath->Tick(fSecond * 1000.f, false);
    }

    if (m_eStatus == GLS_SELECT_GAMEWORLD_SERVER) {
        // �򵼵�����
        OnGuideNetProcess(fSecond);
    } else {
        // ��½������
        this->Process();
    }

    //���·��ֶ���

    if (m_eStatus == GLS_CREATE_CHAR) {
        // Modified by Wayne Wong 2010-11-26 for demo
        for (size_t index = 0; index < m_selectUserCharIds.size(); ++index) {
            if (m_selectUserCharIds[index] == ShuShi) {
                if (m_pSsJian && m_pSsJianPath) {
                    RtgMatrix12 tmp;
                    m_pSsJianPath->GetBoneMat(&tmp, "Blink0");
                    tmp = tmp * (*m_creatActorList[index]->GetWorldMatrix());
                    m_pSsJian->SetMatrix(tmp);
                }
                break;
            }
        }
    }
    fDiffSecond += fSecond;
    if (m_eStatus == GLS_SELECT_CHAR || m_eStatus == GLS_CREATE_CHAR) {
        for (i = 0; i < m_creatActorList.size(); i++) {
            pActor = m_creatActorList[i];
            if (pActor) {
                //ʹ����ʼ��������Ļ
                const RtgMatrix12* oldWorldMatrix;
                RtgMatrix12        newWorldMatrix;
                oldWorldMatrix = m_creatActorList[i]->GetWorldMatrix();
                newWorldMatrix.SetRow(3, *oldWorldMatrix->Row(3));
                m_creatActorList[i]->SetWorldMatrix(newWorldMatrix);

                if (!pActor->IsPlayingPose()) {
                    int actorID = m_selectUserCharIds[i];
                    if (actorID == ZhanShi)  // (charIndex == 0)
                    {
                        pActor->PlayPose("wait_t_sin", false);
                    } else if (actorID == FengWu)  // (charIndex == 1)
                    {
                        pActor->PlayPose("wait_login", false);
                    } else if (actorID == ShuShi)  // (charIndex == 2)
                    {
                        m_pSsJianPath->PlayPose("wait_j0", false);
                        pActor->PlayPose("wait_login", false);
                    } else if (actorID == DaoShi)  // (charIndex == 3)
                    {
                        m_pDsLun->PlayPose("login_wait_n0", false);
                        pActor->PlayPose("wait_login", false);
                    }
                }
                pActor->Tick(fSecond * 1000.f);
            }
        }
        for (i = 0; i < m_selectUserCharActs.size(); i++) {
            if (m_selectUserCharActs[i]) {
                if (m_selectUserCharActs[i]->GetGraph() &&
                    m_selectUserCharActs[i]->GetGraph()->p()) {
                    if (!m_selectUserCharActs[i]->GetGraph()->p()->IsPlayingPose()) {
                        m_selectUserCharActs[i]->mBaseActor.PlayPose(GcBaseActor::POSE_STAND);
                    }
                }
                m_selectUserCharActs[i]->Run(fSecond);
            }
        }

        if (fDiffSecond > 0.1f) {
            fDiffSecond = 0.0f;
            if (!g_workspace.IsMouseMonopolist() && m_bCanInput) {
                GetCursorPos(&pt);
                ScreenToClient((HWND)GetApp()->GetWndHandle(), &pt);
                if (pt.x > 0 && pt.x < GetDevice()->m_iWndWidth && pt.y > 0 &&
                    pt.y < GetDevice()->m_iWndHeight && GetDevice()->HasFocus()) {
                    GetDevice()->GetPickRayFromScreen(pt.x, pt.y, vOrig, vDir);
                    if (m_creatActorList.size()) {
                        for (i = 0; i < m_creatActorList.size(); i++) {
                            if (m_creatActorList[i] == NULL)
                                continue;
                            m16.Set4X3Matrix(*(m_creatActorList[i])->GetWorldMatrix());
                            m16.Invert();
                            v0 = vOrig;
                            v1 = vOrig + vDir * 1000.f;  // ֻ�ж�1000����Զ������
                            v0 = v0 * m16;
                            v1 = v1 * m16;

                            pAABB = (m_creatActorList[i])->GetStaticBoundingBox();
                            if (pAABB == NULL)
                                continue;

                            vMin = pAABB->Min();
                            vMax = pAABB->Max();
                            if (rtgIsLineIntersectAABB(v0, v1, vMin, vMax, &r1, &r2)) {
                                break;
                            }
                        }
                        v0.Set(0.f);
                        v1.Set(1.f);
                        for (j = 0; j < m_creatActorList.size(); j++) {
                            if (m_creatActorList[j] == NULL)
                                continue;
                            if (i != j) {
                                m_creatActorList[j]->SetBaseColor(v0, true);
                            } else {
                                m_creatActorList[j]->SetBaseColor(v1, true);
                            }
                        }
                        if (m_iCurSelectChar >= 0 && m_iCurSelectChar != i) {
                            if (m_creatActorList.size() > m_iCurSelectChar &&
                                m_creatActorList[m_iCurSelectChar]) {
                                m_creatActorList[m_iCurSelectChar]->SetBaseColor(v1, true);
                            }
                        }
                    }

                    if (m_selectUserCharActs.size()) {
                        for (i = 0; i < m_selectUserCharActs.size(); i++) {
                            if (m_selectUserCharActs[i] == NULL)
                                continue;
                            m16.Set4X3Matrix(
                                *(m_selectUserCharActs[i])->GetGraph()->p()->GetWorldMatrix());
                            m16.Invert();
                            v0 = vOrig;
                            v1 = vOrig + vDir * 1000.f;  // ֻ�ж�1000����Զ������
                            v0 = v0 * m16;
                            v1 = v1 * m16;

                            pAABB =
                                (m_selectUserCharActs[i])->GetGraph()->p()->GetStaticBoundingBox();
                            if (pAABB == NULL)
                                continue;

                            vMin = pAABB->Min();
                            vMax = pAABB->Max();
                            if (rtgIsLineIntersectAABB(v0, v1, vMin, vMax, &r1, &r2)) {
                                break;
                            }
                        }
                        v0.Set(0.f);
                        v1.Set(1.f);
                        for (j = 0; j < m_selectUserCharActs.size(); j++) {
                            if (m_selectUserCharActs[j] == NULL)
                                continue;
                            if (i != j) {
                                m_selectUserCharActs[j]->GetGraph()->SetBaseColor(v0);
                            } else {
                                m_selectUserCharActs[i]->GetGraph()->SetBaseColor(v1);
                            }
                        }
                        if (m_iCurSelectChar >= 0 && m_iCurSelectChar != i) {
                            if (m_selectUserCharActs.size() > m_iCurSelectChar &&
                                m_selectUserCharActs[m_iCurSelectChar]) {
                                m_selectUserCharActs[m_iCurSelectChar]->GetGraph()->SetBaseColor(
                                    v1);
                            }
                        }
                    }
                }
            }
        }
    }
    //UI��Ⱦ
    g_workspace.Run(fSecond * 1000);
    unguard;
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

void GcLogin::OnRender(float fSecond) {
    guard;
    GameSetCursor();

    GetDevice()->SetLight(0, &m_lightDirect);
    if (m_bLight2) {
        GetDevice()->SetLight(1, &m_lightDirect2);
        GetDevice()->SetLight(2, NULL);
    } else {
        GetDevice()->SetLight(1, NULL);
    }

    if (m_eStatus == GLS_LOADING) {
        if (m_bLoading) {
            OnLoading();
            SetLoginState(GLS_SELECT_GAMEWORLD_SERVER);
        } else {
            m_bLoading = true;
        }
    }

    RtgMatrix16 m16;
    m16.Unit();
    //
    GetDevice()->SetMatrix(RTGTS_WORLD, &m16);
    GetDevice()->SetRenderState(RTGRS_Z_TEST, TRUE);

    OnRenderMask(RTGRM_NOBLENDOBJ_ONLY, fSecond);
    OnRenderMask(RTGRM_BLENDOBJ_ONLY, fSecond);
    unguard;
}

void GcLogin::OnRenderMask(RTGRenderMask mask, float fSecond) {
    guard;
    CRT_ActorInstance*                                  pActor;
    std::map<std::string, CRT_ActorInstance*>::iterator it;

    RtgCamera* pCamera = GetDevice()->m_pCamera;

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

    if (m_eStatus == GLS_SELECT_CHAR || m_eStatus == GLS_CREATE_CHAR) {
        for (int i = 0; i < m_creatActorList.size(); i++) {
            pActor = m_creatActorList[i];
            if (pActor) {
                pActor->Render(GetDevice(), mask, true, true);
            }
        }
        for (int i = 0; i < m_selectUserCharActs.size(); i++) {
            if (m_selectUserCharActs[i]) {
                m_selectUserCharActs[i]->Render(*GetDevice(), mask);
            }
        }
    }
    if (m_eStatus == GLS_CREATE_CHAR) {
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

int l = 1;
int m = 0;
int r = 2;

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

    switch (m_eStatus) {
        case GLS_SELECT_CHAR:
            for (i = 0; i < m_selectUserCharActs.size(); i++) {
                if (m_selectUserCharActs[i] == NULL)
                    continue;
                m16.Set4X3Matrix(*(m_selectUserCharActs[i])->GetGraph()->p()->GetWorldMatrix());
                m16.Invert();
                v0 = vOrig;
                v1 = vOrig + vDir * 1000.f;  // ֻ�ж�1000����Զ������
                pAABB = (m_selectUserCharActs[i])->GetGraph()->p()->GetStaticBoundingBox();
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
#ifdef _DEBUG
    GetDevice()->m_pCamera->AddYaw(DegreeToRadian(increaseX));
    GetDevice()->m_pCamera->AddRoll(DegreeToRadian(-increaseY));
#endif
#ifdef _PREVIEW

    GetDevice()->m_pCamera->AddYaw(DegreeToRadian(increaseX));
    GetDevice()->m_pCamera->AddRoll(DegreeToRadian(-increaseY));
#endif
    if (m_eStatus == GLS_CREATE_CHAR) {
        if (m_iCurSelectChar == 0 || m_iCurSelectChar == 1) {
            m_creatActorList[m_iCurSelectChar]->RotateLZ(DegreeToRadian(increaseX / 5));
            /*RtgMatrix12* pActorMatrix;
			pActorMatrix = m_creatActorList[m_iCurSelectChar]->GetMatrix();
			if (pActorMatrix)
			{
				pActorMatrix->RotateLZ();
			}*/
        }
    }
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

void GcLogin::OnKeyDown(int iButton, int iKey) {
    guard;
    //lyymark 2.GcLogin ��v��ʾ��ǰViewMatrix
    P_LOGINFO("key: \n" + std::to_string(iButton) + " " + std::to_string(iKey));
#ifdef _PREVIEW
    if (iButton == 86) {
        auto gvm = GetDevice()->m_pCamera->GetViewMatrix();
        P_LOGINFO("ViewMatrix: \n" + gvm.ToString());
    }
#else
    if (!m_bCanInput)
        return;
#endif
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
    //������ʾ��ǰѡ��ķ���������
    g_layerLogin->mp_selectServerName->SetText(m_szGameWorldServerName);
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

        m_bSelectCharDone = true;
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
        if (!m_bSelectCharDone) {
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
    m_bSelectCharDone = true;
}

//�ı������ɫ����
void GcLogin::ChangeCharHair(bool bNext) {
    if (m_iCurSelectChar < 0)
        return;

    int iCharIndex = GetCharIndexByActorID(m_selectUserCharIds[m_iCurSelectChar]);

    vector<SHeadModel> heads;
    if (!g_TableHeadModel.GetHeadModel(s_userID[iCharIndex], heads))
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

int GcLogin::GetPing(char* zIP) {
    guard;
    SECURITY_ATTRIBUTES sa, sa2;
    HANDLE              hInputRead, hInputWrite;
    HANDLE              hOutputRead, hOutputWrite;

    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;
    if (!CreatePipe(&hOutputRead, &hOutputWrite, &sa, 0)) {
        return 1;
    }

    sa2.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa2.lpSecurityDescriptor = NULL;
    sa2.bInheritHandle = TRUE;
    if (!CreatePipe(&hInputRead, &hInputWrite, &sa2, 0)) {
        return 1;
    }

    STARTUPINFO         si;
    PROCESS_INFORMATION pi;
    si.cb = sizeof(STARTUPINFO);
    GetStartupInfo(&si);
    si.hStdError = hOutputWrite;
    si.hStdOutput = hOutputWrite;
    si.hStdInput = hInputRead;
    si.wShowWindow = SW_HIDE;
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

    char cmd_line[100];
    rt2_sprintf(cmd_line, TEXT("ping %s"), zIP);
    if (!CreateProcess(NULL, cmd_line, NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi)) {
        return 1;
    }

    CloseHandle(hInputRead);
    CloseHandle(hOutputWrite);

    char  buffer[4096] = {0};
    char  bufferStart[] = " time";
    char  bufferEnd[] = "ms ";
    char  ping[10] = {0};
    DWORD bytesRead;
    int   i = 1;
    int   pingNum = 0;
    while (true) {
        if (ReadFile(hOutputRead, buffer, 4095, &bytesRead, NULL) == NULL) {
            break;
        }
        // �ַ�������
        // ��һ�β���
        char* startPos = strstr(buffer, bufferStart);
        while (startPos) {
            char* endPos = strstr(startPos, bufferEnd);
            if (endPos) {
                memcpy(ping, startPos + 6, endPos - startPos - 6);
                pingNum += atoi(ping);
                i++;
            }
            char* startPos = strstr(endPos + 3, bufferStart);
            if (i > 4)
                break;
        }
        Sleep(500);
        if (i > 0)
            return (int)(pingNum / i);
    }
    CloseHandle(hInputWrite);
    CloseHandle(hOutputRead);
    return (int)(pingNum / i);
    unguard;
}

//lyymark 2.GcLogin.UI.OnUIUpdateGameWorldServerList ������Ϸ����������б�
void GcLogin::OnUIUpdateGameWorldServerList() const {
    guard;

    if (m_eStatus != GLS_SELECT_GAMEWORLD_SERVER)
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
    if (m_eStatus == GLS_CREATE_CHAR) {
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
    if (m_pCamera) {
        m_pCamera = NULL;
    }
    iAnimalIndex = rand() % 12;
    iRandomAnimalIndex = iAnimalIndex;
    RtwImage* pImage = g_workspace.getImageFactory()->createImage("ui/textures/11.tga");
    RtwImage* pImage1 = g_workspace.getImageFactory()->createImage("ui/textures/11_a.tga");
    if (m_eStatus == GLS_CREATE_CHAR) {
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
            pImage = g_workspace.getImageFactory()->createImage(s_pszCharAnimalName[iAnimalIndex]);
            pImage->SetBlend(true);
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
            pImage = g_workspace.getImageFactory()->createImage(s_pszCharAnimalName[iAnimalIndex]);
            pImage->SetBlend(true);
            LOAD_UI("fmcreatid1.fmanimal.lbanimal")->SetBackgroundImage(pImage);
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
            pImage = g_workspace.getImageFactory()->createImage(s_pszCharAnimalName[iAnimalIndex]);
            pImage->SetBlend(true);
            LOAD_UI("fmcreatid1.fmanimal.lbanimal")->SetBackgroundImage(pImage);
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
            pImage = g_workspace.getImageFactory()->createImage(s_pszCharAnimalName[iAnimalIndex]);
            pImage->SetBlend(true);
            LOAD_UI("fmcreatid1.fmanimal.lbanimal")->SetBackgroundImage(pImage);
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

    for (size_t i = 0; i < sizeof(s_userID) / sizeof(s_userID[0]); ++i) {
        if (s_userID[i] == actorID)
            return i;
    }
    return -1;
}

const char* GcLogin::GetCameraPoseName(int iSel, bool bSelectChar) {
    switch (iSel) {
        case 0:  //��̨��
            switch (m_iCurSelectChar) {
                case -1:
                    return bSelectChar ? "otoo1" : "otoow";
                case 0:
                    break;
                case 1:
                    return bSelectChar ? "2too1" : "htoow";
                case 2:
                    return bSelectChar ? "3too1" : "stoow";
                case 3:
                    return bSelectChar ? "4too1" : "mtoow";
                default:
                    break;
            }
            break;
        case 1:  //����Ů
            switch (m_iCurSelectChar) {
                case -1:
                    return bSelectChar ? "otoo2" : "otooh";
                case 0:
                    return bSelectChar ? "1too2" : "wtooh";
                case 1:
                    break;
                case 2:
                    return bSelectChar ? "3too2" : "stooh";
                case 3:
                    return bSelectChar ? "4too2" : "mtooh";
                default:
                    break;
            }
            break;
        case 2:  //��ɽ��
            switch (m_iCurSelectChar) {
                case -1:
                    return bSelectChar ? "otoo3" : "otoos";
                case 0:
                    return bSelectChar ? "1too3" : "wtoos";
                case 1:
                    return bSelectChar ? "2too3" : "htoos";
                case 2:
                    break;
                case 3:
                    return bSelectChar ? "4too3" : "mtoos";
                default:
                    break;
            }
            break;
        case 3:  //�置Ů
            switch (m_iCurSelectChar) {
                case -1:
                    return bSelectChar ? "otoo4" : "otoom";
                    break;
                case 0:
                    return bSelectChar ? "1too4" : "wtoom";
                    break;
                case 1:
                    return bSelectChar ? "2too4" : "htoom";
                    break;
                case 2:
                    return bSelectChar ? "3too4" : "stoom";
                    break;
                case 3:
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }

    return NULL;
}

CRT_ActorInstance* GcLogin::GetSelectedActorByActorID(short actorID) {
    for (size_t index = 0; index < m_selectUserCharIds.size(); ++index) {
        if (m_selectUserCharIds[index] == actorID)
            return m_creatActorList[index];
    }

    return NULL;
}

/*----------------------------------------------------------------------------
-   End.
----------------------------------------------------------------------------*/

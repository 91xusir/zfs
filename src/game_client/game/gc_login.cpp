#include "gc_include.h"
#include "gc_cursor.h"
#include "gc_login.h"
#include "region_client.h"
#include "gc_login_session.h"
#include "game_script_interface.h"
#include "gc_constant.h"
#include "gs_protocol.h"
#include "lgs_gws.h"
#include "trump_show.h"
#include "ui_form_msg.h"
#include "ui_form_textMsg.h"
#include "UIForm_Server.h"
#include "soft_keyboard.h"
#include "../gc_fps.h"
#include <typeinfo>
//1--五台 4--花间 7--蜀山 10--苗疆 13--蜀山(女) 16--苗疆(女)
//static int s_userID[] = {1, 4, 7, 10, 13, 16};
//static char* s_pszLoginCharName[] = {"role_wt_m_01", "role_hj_f_01", "role_ss_m_01", "role_mj_m_01", "role_ss_f_01", "role_mj_f_01"};
//static char* s_pszCharCreateBody[] = {"a_wt_m_clo_04", "a_hj_f_clo_04", "a_ss_m_clo_04", "a_mj_m_clo_04", "a_ss_f_clo_04","a_mj_f_clo_04"};
//static char* s_pszCharCreateLeg[] = {"a_wt_m_sho_04", "a_hj_f_sho_04", "a_ss_m_sho_04", "a_mj_m_sho_04", "a_ss_f_sho_04", "a_mj_f_sho_04"};
//static char* s_pszCharCreateHand[] = {"a_wt_m_glo_04", "a_hj_f_glo_04", "a_ss_m_glo_04", "a_mj_m_glo_04", "a_ss_f_glo_04", "a_mj_f_glo_04"};
//static char* s_pszCharCreateWaist[] = {"a_wt_m_wai_04", "a_hj_f_wai_04", "a_ss_m_wai_04", "a_mj_m_wai_04", "a_ss_f_wai_04", "a_mj_f_wai_04"};

#define WU_TAI_ID 1
#define HUA_JIAN_ID 4
#define SHU_SHAN_ID 7
#define MIAO_JIANG_ID 16
static int s_userID[] = { WU_TAI_ID, HUA_JIAN_ID, SHU_SHAN_ID, MIAO_JIANG_ID };

static char* s_pszLoginCharName[] = { "role_wt_m_01", "role_hj_f_01", "role_ss_m_01", "role_mj_f_01" };
static char* s_pszCharCreateBody[] = { "a_wt_m_clo_07_03", "a_hj_f_clo_07_01", "a_ss_m_clo_07_04", "a_mj_f_clo_04_04" };
static char* s_pszCharCreateLeg[] = { "a_wt_m_sho_07_03", "a_hj_f_sho_07_01", "a_ss_m_sho_07_04",  "a_mj_f_sho_04_04" };
static char* s_pszCharCreateHand[] = { "a_wt_m_glo_07_03", "a_hj_f_glo_07_01", "a_ss_m_glo_07_04", "a_mj_f_glo_04_04" };
static char* s_pszCharCreateWaist[] = { "a_wt_m_wai_07_03", "a_hj_f_wai_07_01", "a_ss_m_wai_07_04", "a_mj_f_wai_04_04" };
static char* s_pszCharAnimalName[] =
{
	"ui/textures/animal1.tga","ui/textures/animal2.tga","ui/textures/animal3.tga","ui/textures/animal4.tga","ui/textures/animal5.tga","ui/textures/animal6.tga",
	"ui/textures/animal7.tga","ui/textures/animal8.tga","ui/textures/animal9.tga","ui/textures/animal10.tga","ui/textures/animal11.tga","ui/textures/animal12.tga"
};
std::string GcLogin::m_szAccountUsername;
std::string GcLogin::m_szAccountPassword;
CLockFrame lockFrame(1000 / 60);
GcLogin::GcLogin(CGameClientFrame* pGameClientFrame) :m_ini(true)
{
	m_eStatus = GLS_NONE;
	m_eNextStatus = GLS_NONE;
	m_pGameClientFrame = pGameClientFrame;
	m_bSelectCharDone = false;
	m_bCanInput = false;
	m_iCurSelectChar = -1;
	m_bSex = SEX_MALE;
	m_pBody = NULL;
	m_pCamera = NULL;
	m_pWeaponWT = NULL;
	m_pWeaponFL = NULL;
	m_pWeaponFLWay = NULL;
	m_pWeaponHL = NULL;
	m_pWeaponMJ = NULL;
	m_pPetActor1 = NULL;
	m_pPetActor2 = NULL;
	m_pPetActor3 = NULL;
	for (int i = 0; i < 4; i++)
		m_HeadModelIndex[i] = 0;
	for (int i = 0; i < 4; i++)
		m_HeadImageIndex[i] = 0;
	for (int i = 0; i < 4; i++)
		m_bCharFrozen[i] = 0;

	m_szAccountPassword = "";
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

GcLogin::~GcLogin()
{
}

void GcLogin::LoginErrMsg(EErrMsg eMsg, const char* szRetStr, short sRetCode)
{
	const char* pMsg;
	switch (eMsg)
	{
	case GLEM_DISCONNECT:                   pMsg = R(LMSG_DISCONNECT); break;                     //登录连接中断
	case GLEM_CANNOT_CONNECT:               pMsg = R(LMSG_CANNOT_CONNECT); break;                 //不能连接网络
	case GLEM_CONNECT_FAIL:                 pMsg = R(LMSG_CONNECT_FAIL); break;                   //连接登录服务器失败
	case GLEM_LOGIN_FAIL_NOT_FOUND:         pMsg = R(LMSG_LOGIN_FAIL_NOT_FOUND); break;           //用户名不存在
	case GLEM_LOGIN_FAIL_PWD_WRONG:         pMsg = R(LMSG_LOGIN_FAIL_PWD_WRONG); break;			  //登录失败,请检查您的密码和验证码是否正确
	case GLEM_LOGIN_FAIL:                   pMsg = R(LMSG_LOGIN_FAIL); break;					  //登录失败
	case GLEM_LOGIN_FAIL_USER_ONLINE:	    pMsg = R(LMSG_LOGIN_FAIL_USER_ONLINE); break;		  //帐号正在被使用
	case GLEM_LOGIN_FAIL_ACCOUNT_STOP:	    pMsg = R(LMSG_LOGIN_FAIL_ACCOUNT_STOP); break;		  //帐号被封停
	case GLEM_LOGIN_FAIL_ACCOUNT_PAUSE:	    pMsg = R(LMSG_LOGIN_FAIL_ACCOUNT_PAUSE); break;		  //密码错误
	case GLEM_LOGIN_FAIL_ACCOUNT_WRONG5:    pMsg = R(LMSG_LOGIN_FAIL_ACCOUNT_WRONG5); break;       //由于连续输入帐号密码错误，为保障用户信息安全，锁定帐号10分钟
	case GLEM_LOGIN_FAIL_POT_NOT_ENOUGH:    pMsg = R(LMSG_LOGIN_FAIL_POT_NOT_ENOUGH); break;	  //帐号点数不足
	case GLEM_GET_ACCOUNT_FAIL:             pMsg = R(LMSG_GET_ACCOUNT_FAIL); break;				  //获取帐号信息失败
	case GLEM_CREATE_RESULT_EXIST:          pMsg = R(LMSG_CREATE_RESULT_EXIST); break;			  //角色已经存在
	case GLEM_CREATE_RESULT_FAIL:           pMsg = R(LMSG_CREATE_RESULT_FAIL); break;			  //创建角色失败
	case GLEM_CREATE_FACTION_ERROR:       pMsg = R(LMSG_CREATE_RESULT_FACTION_ERROR); break;	  //创建角色失败,只能创建同一阵营的角色
	case GLEM_CREATE_RESULT_INVALID_NAME:   pMsg = R(LMSG_CREATE_RESULT_INVALID_NAME); break;	  //创建角色失败,角色名称最长为八个字符
	case GLEM_CREATE_BANNED_NAME:           pMsg = R(LMSG_CREATE_CHAR_NAME_BANNED); break;        //角色名称包含违禁字符
	case GLEM_RESTORE_RESULT_FAIL:          pMsg = R(LMSG_RESTORE_RESULT_FAIL); break;              //恢复角色失败
	case GLEM_DELETE_RESULT_FAIL:           pMsg = R(LMSG_DELETE_RESULT_FAIL); break;             //删除角色失败
	case GLEM_LOGIN_FAIL_SERVER_FULL:       pMsg = R(LMSG_LOGIN_FAIL_SERVER_FULL); break;         //服务器已满,请稍后再试
	case GLEM_LOGIN_FAIL_SERVER_STOP:       pMsg = R(LMSG_LOGIN_FAIL_SERVER_STOP); break;         //服务器维护中
	case GLEM_LOGIN_FAIL_NEW_CARD:
		pMsg = R(LMSG_LOGIN_FAIL_NEW_CARD);
		PostQuitMessage(22);
		break;
	case GLEM_LOGIN_FAIL_PUBWIN_CENTER:		pMsg = R(LMSG_LOGIN_FAIL_PUBWIN_CENTER); break;        //同步会员中心失败,请稍后再试
	case GLEM_LOGIN_FAIL_IS_LOCKED:			pMsg = R(LMSG_LOGIN_FAIL_IS_LOCKED); break;            //帐号被锁定
	case GLEM_NONE:
	default: pMsg = R(LMSG_LOGIN_FAIL_UNKNOWE); break;
	}
	ERR1("%s\n", pMsg);
	if (szRetStr)
	{
		rt2_sprintf(g_strStaticBuffer, "%s [%s]", pMsg, szRetStr);
		ShowMessage(g_strStaticBuffer);
	}
	else if (sRetCode)
	{
		if (sRetCode == 110)
		{
			UIFormMsg* pFrm = UIFormMsg::ShowStatic(pMsg, UIFormMsg::TYPE_OK_CANCEL);
			pFrm->EvOK = RTW_CALLBACK_1(g_layerLogin, UILayerLogin, OnClicked_ForceLogin, pFrm);
			pFrm->EvCancel = RTW_CALLBACK_1(g_layerLogin, UILayerLogin, OnClicked_ForceLoginCancel, pFrm);
		}
		else
		{
			rt2_sprintf(g_strStaticBuffer, "%s", pMsg);
			ShowMessage(g_strStaticBuffer);
		}
	}
	else
	{
		ShowMessage(pMsg);
	}
}

//lyymark 2.GcLogin.LoadLoginSection 加载login.ini配置文件
void LoadLoginSection(RtIni* pIni, const char* szSectionName,
	std::map<std::string, CRT_ActorInstance*>& mapActor)
{
	guard;
	RtString szLink, szName;
	CRT_ActorInstance* pBody, * pActor;
	pBody = nullptr;
	// 查找INI文件中指定名称的节
	pIni->FindSection(szSectionName);
	// 获取第一个条目
	if (pIni->FirstEntry(&szLink, &szName))
	{
		do {
			// 创建一个actor实例
			pActor = RtcGetActorManager()->CreateActor(szName, true);
			if (pActor)
			{
				// 如果actor名称不以"role"开头，则播放动作
				if (strncmp(pActor->m_Name.c_str(), "role", 4) != 0)
				{
					pActor->PlayPose(true);
				}
				// 将actor实例添加到映射中
				mapActor[std::string(szLink)] = pActor;
				// 如果链接名称为"Body"，则将当前actor实例设为主体
				if (szLink == "Body")
				{
					pBody = pActor;
				}
				else
				{
					// 直接链接非 "Body" 的 actor 实例
					if (pBody) // 确保 pBody 已被设置
					{
						pActor->LinkParent(pBody, szLink.c_str());
					}
				}
			}
		} while (pIni->NextEntry(&szLink, &szName));
	}
	// 循环和上面合二为一优化   lyy [2024.7.25]
	//if (pBody)
	//{

	//	for (auto& pair : mapActor)
	//	{
	//		if (pair.first != "Body")
	//		{
	//			auto& act = pair.second;
	//			act->LinkParent(pBody, pair.first.c_str());
	//		}
	//	}

	//}
	unguard;
}


bool GcLogin::InitOnce()
{
	guard;
	return true;
	unguard;
}

bool GcLogin::ClearOnce()
{
	guard;
	{
		std::map<std::string, CRT_ActorInstance*>::iterator it;
		for (it = m_mapLogin.begin(); it != m_mapLogin.end(); it++)
		{
			RtcGetActorManager()->ReleaseActor((*it).second);
		}
		for (it = m_mapSelectChar.begin(); it != m_mapSelectChar.end(); it++)
		{
			RtcGetActorManager()->ReleaseActor((*it).second);
		}
		for (it = m_mapCreateChar.begin(); it != m_mapCreateChar.end(); it++)
		{
			if ((*it).first == "s_dahuangdi")
			{
				int a = 1;
			}
			RtcGetActorManager()->ReleaseActor((*it).second);
		}
	}

	{
		std::map<DWORD, GcActor*>::iterator itActor;
		for (itActor = m_mapSelectActor.begin(); itActor != m_mapSelectActor.end(); itActor++)
		{
			DEL_ONE((*itActor).second);
		}
	}

	RtcGetActorManager()->ReleaseActor(m_pWeaponWT);
	RtcGetActorManager()->ReleaseActor(m_pWeaponFL);
	RtcGetActorManager()->ReleaseActor(m_pWeaponFLWay);
	RtcGetActorManager()->ReleaseActor(m_pWeaponHL);
	RtcGetActorManager()->ReleaseActor(m_pWeaponMJ);
	RtcGetActorManager()->ReleaseActor(m_pPetActor1);
	RtcGetActorManager()->ReleaseActor(m_pPetActor2);
	RtcGetActorManager()->ReleaseActor(m_pPetActor3);

	GetDevice()->m_dwClearColor = 0x00000000;

	UIFormTextMsg::Clear();
	UIFormMsg::Clear(false);

	return true;
	unguard;

}
//lyymark 2.GcLogin 登录状态机
void GcLogin::SetLoginState(EStatus eState)
{
	guard;
	m_ePrevStatus = m_eStatus;
	m_eNextStatus = eState;

	// 离开
	bool bLeaveNow = true;
	switch (m_eStatus)
	{
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

	// 设置
	if (!bLeaveNow)
	{
		return;
	}
	else
	{
		m_eNextStatus = GLS_NONE;
		m_eStatus = eState;
	}

	// 进入
	switch (m_eStatus)
	{
	case GLS_LOADING:
		EnterLoading();
		break;
	case GLS_SELECT_GAMEWORLD_SERVER:
		if (m_eStatus != GLS_LOGIN)
		{
			//当前mapActor组设置为登录act组
			m_mapActor = m_mapLogin;
		}
		EnterSelectGameWorldServer();

		break;
	case GLS_LOGIN:
		if (m_eStatus != GLS_SELECT_GAMEWORLD_SERVER)
		{
			m_mapActor = m_mapLogin;
		}
		EnterLogin();
		break;
	case GLS_SELECT_CHAR:
		m_mapActor = m_mapSelectChar;
		EnterSelectChar();
		break;
	case GLS_CREATE_CHAR:
		m_mapActor = m_mapCreateChar;
		EnterCreateChar();
		break;
	}
	if (m_eStatus != GLS_NONE)
	{
		if (m_eStatus == GLS_SELECT_GAMEWORLD_SERVER || m_eStatus == GLS_LOGIN)
		{
			SJDL_UpdateCameraPos();
		}
		else {
			UpdateCameraPos();
		}
	}
	unguard;
}

CRT_ActorInstance* GcLogin::FindModel(const std::string& szName)
{
	guard;
	auto it = m_mapActor.find(std::string(szName));
	if (it != m_mapActor.end()) return it->second;
	return NULL;
	unguard;
}

namespace {

	/**
	由于模型骨骼矩阵是 4x3 矩阵（RtgMatrix12），
	而相机矩阵是 4x4 矩阵（RtgMatrix16），
	需要一个转换函数 _AdjustCameraMatrix。
	*/
	inline void _AdjustCameraMatrix(RtgMatrix16* matOut, RtgMatrix12* matIn)
	{
		RTASSERT(matOut);
		RTASSERT(matIn);

		RtgVertex3 z = *(RtgVertex3*)&matIn->_10;
		z.Normalize();
		z.Negative();
		RtgVertex3 x = RtgVertex3(0.f, 0.f, 1.f).Cross(z);
		x.Normalize();
		RtgVertex3 y = z.Cross(x);
		y.Normalize();
		//设置为单位矩阵
		matOut->Identity();
		matOut->SetRow(0, x);
		matOut->SetRow(1, y);
		matOut->SetRow(2, z);
		matOut->SetRow(3, (float*)&matIn->_30);
	}
}
// 更新相机位置
//BOOKMARK: this is a bookmark
void GcLogin::UpdateCameraPos()
{
	guard;
	RtgMatrix16 mCamera;

	CRT_ActorInstance* pCamera;
	m_pBody = FindModel("Body");
	pCamera = m_pBody;

	if (pCamera)
	{
		RtgMatrix12 _mat;
		if (m_pBody->GetBoneMat(&_mat, "bcam"))
		{
			_AdjustCameraMatrix(&mCamera, &_mat);
			GetDevice()->m_pCamera->SetMatrix(mCamera);
		}
	}
	unguard;
}
void  GcLogin::SJDL_UpdateCameraPos()
{
	guard;
	RtgMatrix16 mCamera;
	CRT_ActorInstance* pCamera;
	m_pBody = FindModel("Body");
	pCamera = m_pBody;
	if (pCamera)
	{
		RtgMatrix12 _mat;
		if (m_pBody->GetBoneMat(&_mat, "bcam"))
		{
			RTASSERT(&mCamera);
			RTASSERT(&_mat);
			RtgVertex3 x = *(RtgVertex3*)&_mat._00;
			RtgVertex3 y = *(RtgVertex3*)&_mat._10;
			RtgVertex3 z = *(RtgVertex3*)&_mat._20;
			x.Normalize();
			y.Normalize();
			z.Normalize();
			mCamera.Identity();
			mCamera.SetRow(0, x);
			mCamera.SetRow(1, y);
			mCamera.SetRow(2, z);
			mCamera.SetRow(3, (float*)&_mat._30);
			GetDevice()->m_pCamera->SetMatrix(mCamera);
			P_LOGINFO("BodyMatrix:\n" + _mat.ToString());
			P_LOGINFO("CameraMatrix:\n" + mCamera.ToString());
		}
	}
	unguard;
}

void GcLogin::EnterSelectGameWorldServer()
{
	guard;
	if (g_layerLogin == NULL)
	{
		//lyymark 2.GcLogin 打开登录UI
		UILayer::EnterLogin();
	}

	UpdateGraphConfig("Graph_Login");

	if (m_pCamera)
	{
		m_pCamera = NULL;
	}

	if (m_pCamera = FindModel("Camera"))
	{
		m_pCamera->RegisterNotify(this);
		if (!m_pCamera->PlayPose("idle", true))
			m_pCamera->RegisterNotify(NULL);
	}

	//	GetDevice()->m_pCamera->MoveForward(100 * 100.0f);

	StartGetGameWorldServer();

	LOAD_UI("btnback")->Hide();
	LOAD_UI("btnexit")->Show();
	//领取密保
	LOAD_UI("btngetpwd")->Hide();
	//忘记密码
	LOAD_UI("btnforgetpwd")->Hide();
	//申请帐号
	LOAD_UI("btncreate")->Hide();
	if (m_Keyboard.IsVisible())
	{
		m_Keyboard.HideSoftKeyboard();
	}

	RTW_WIDGET("fmserver")->Show();

	unguard;
}

bool GcLogin::LeaveSelectGameWorldServer()
{
	guard;
	CHECK(m_eStatus == GLS_SELECT_GAMEWORLD_SERVER);
	RTW_WIDGET("fmserver")->Hide();
	EndGetGameWorldServer();
	return true;
	unguard;
}

void GcLogin::EnterLogin()
{
	guard;
	if (g_layerLogin == NULL)
	{
		UILayer::EnterLogin(); // 打开登陆界面
	}

	UpdateGraphConfig("Graph_Login");
	if (m_pCamera)
	{
		m_pCamera = NULL;
	}
	if (m_pCamera = FindModel("Camera"))
	{
		m_pCamera->RegisterNotify(this);
		if (!m_pCamera->PlayPose("idle", true))
			m_pCamera->RegisterNotify(NULL);
	}

	LOAD_UI("btnback")->Show();
	LOAD_UI("btnexit")->Show();
	//领取密保
	LOAD_UI("btngetpwd")->Show();
	//忘记密码
	LOAD_UI("btnforgetpwd")->Show();
	//申请帐号
	LOAD_UI("btncreate")->Show();

	// 读取用户名和密码
	RtIni iniUser;
	const char* szSave = 0;
	int iSave;
	const char* szIniUsername = 0;
	const char* szIniPassword = 0;

	if (iniUser.OpenFile(R(INI_USER)))
	{
		szSave = iniUser.GetEntry("account", "save");
		if (szSave)
		{
			iSave = atol(szSave);
			if (iSave > 0)
			{
				g_layerLogin->m_pBtnSaveAccount->SetChecked(true);
				szIniUsername = iniUser.GetEntry("login", "username");
				szIniPassword = iniUser.GetEntry("login", "password");
				if (szIniUsername)
				{
					m_szAccountUsername = szIniUsername;
				}
				else
				{
					szIniUsername = "";
					m_szAccountUsername = szIniUsername;
				}
				if (!szIniPassword)
				{
					szIniPassword = "";
				}
			}
			else
			{
				g_layerLogin->m_pBtnSaveAccount->SetChecked(false);
				szIniUsername = "";
				szIniPassword = "";
				m_szAccountUsername = szIniUsername;
			}
		}
		else
		{
			szIniUsername = "";
			szIniPassword = "";
			m_szAccountUsername = szIniUsername;
		}

		iniUser.CloseFile();
	}

	if (!GetLogin()->m_ishallLogin)
	{
		g_layerLogin->m_username->Enable();
		g_layerLogin->m_password->Enable();
		// 设置控件值
		g_layerLogin->m_username->SetText(std::string(m_szAccountUsername));
		g_layerLogin->m_password->SetText(szIniPassword);

		// 设置输入焦点
		if (m_szAccountUsername.length() == 0)
		{
			g_layerLogin->m_username->SetFocus();
			if (m_Keyboard.IsVisible())
			{
				m_Keyboard.HideSoftKeyboard();
			}
		}
		else
		{
			g_layerLogin->m_password->SetFocus();
			m_Keyboard.ShowSoftKeyboard();
		}

		RTW_WIDGET("fmserver")->Hide();
		RTW_WIDGET("fmlogin")->Show();
		((RtwForm*)RTW_WIDGET("fmlogin"))->SetShowCloseButton(false);
	}
	else
	{
		g_layerLogin->m_username->SetText(m_hallName);
		g_layerLogin->m_password->SetText(m_hallKey);
		RTW_WIDGET("fmserver")->Hide();
		RTW_WIDGET("fmlogin")->Show();
		((RtwForm*)RTW_WIDGET("fmlogin"))->SetShowCloseButton(false);
		g_layerLogin->m_username->Disable();
		g_layerLogin->m_password->Disable();
	}
	unguard;
}

bool GcLogin::LeaveLogin()
{
	guard;
	CHECK(m_eStatus == GLS_LOGIN);
	RTW_WIDGET("fmlogin")->Hide();
	return true;
	unguard;
}

void GcLogin::EnterSelectChar()
{
	guard;
	if (m_pCamera)
	{
		m_pCamera = NULL;
	}
	UILayer::EnterSelectChar();

	UpdateGraphConfig("Graph_SelectChar");
	//开启后处理特效
	GetDevice()->SetPostProcessEffectOnoff(true);
	GetDevice()->SetPostProcessObjectHighLightOnoff(true);

	//获取按钮矩形,调整按钮位置
	rectBtnSetChar = RTW_WIDGET("btnsetcharpwd")->GetClientRect();
	rectBtnChangeChar = RTW_WIDGET("btnchangecharpwd")->GetClientRect();
	RTW_WIDGET("fmckusername")->Show();
	RTW_WIDGET("btnexit")->Show();
	RTW_WIDGET("btnbacklogin")->Show();
	//人物旋转按钮
	//RTW_WIDGET("btnuserleft")->Show();
	//RTW_WIDGET("btnuserright")->Show();

	UpdateCameraPos();
	UpdateSelectChar();

	m_iCurSelectChar = -1;
	m_bCanInput = true;
	unguard;
}

bool GcLogin::LeaveSelectChar()
{
	guard;
	CHECK(m_eStatus == GLS_SELECT_CHAR);

	m_bCanInput = false;

	RTW_WIDGET("fmckusername")->Hide();
	RTW_WIDGET("btnenter")->Hide();
	RTW_WIDGET("btndeletechar")->Hide();
	RTW_WIDGET("btnsetcharpwd")->Hide();
	RTW_WIDGET("btnchangecharpwd")->Hide();
	RTW_WIDGET("btndelcharpwd")->Hide();
	RTW_WIDGET("btnexit")->Hide();
	RTW_WIDGET("btnbacklogin")->Hide();
	//人物旋转按钮
	//RTW_WIDGET("btnuserleft")->Hide();
	//RTW_WIDGET("btnuserright")->Hide();

	m_listSelGcActor.clear();
	m_listSelectChar.clear();
	return true;
	unguard;
}

void GcLogin::UpdateSelectChar()
{
	guard;
	// 检查当前状态是否为选择角色状态，不是则返回
	if (m_eStatus != GLS_SELECT_CHAR) return;

	int i;
	const SUserActor* pUser = NULL;
	GcActor* pActor = NULL;
	const SCreModel* pModel = NULL;
	const GcAccountInfo& info = GetAccountInfo();

	// 遍历用户角色列表
	for (i = 0; i < info.chatCount; i++)
	{
		// 更新角色是否被冻结状态
		m_bCharFrozen[i] = info.users[i].isFrozen;

		// 查找用户角色信息
		pUser = g_TableUserActor.FindUserActor(info.users[i].attributes.actorID);
		if (pUser)
		{
			// 如果在角色选择映射中找不到该角色
			if (m_mapSelectActor.find(info.users[i].id) == m_mapSelectActor.end())
			{
				// 查找角色模型信息
				pModel = g_TableCreModel.FindCreModel(pUser->ModelId);
				if (pModel)
				{
					// 创建新角色对象
					pActor = RT_NEW GcActor;
					pActor->CreateGraphData((SCreModel*)pModel, info.users[i].headModelID);
					pActor->SetName(info.users[i].name);
					pActor->DisplayActor(true);

					// 隐藏角色的HUD（头顶显示信息）    ps：不确定
					if (pActor->m_pHUD)
					{
						pActor->m_pHUD->Hide();
					}

					if (pActor)
					{
						// 设置角色的职业和性别属性
						pActor->m_core.Metier = info.users[i].attributes.metier;
						pActor->m_core.Sex = info.users[i].attributes.Sex;
						// 加载角色的物品信息
						CItemManager* pItemMgr = ((CGameClientFrame*)GetApp())->m_pItemManager;
						if (pItemMgr)
						{
							SItemID item;
							if (pActor->mItem.LoadFromString(info.users[i].items) <= 0)
							{
								// 如果物品信息无效，返回登录状态并显示错误信息
								SetLoginState(GLS_LOGIN);
								ShowMessage(R(LMSG_USERDATA_INVALID));
								return;
							}
						}
					}
				}
				m_mapSelectActor[info.users[i].id] = pActor;
			}
		}
	}

	m_listSelGcActor.clear();
	m_listSelectChar.clear();

	if (info.chatCount == 0)
	{
		RTW_WIDGET("btnenter")->Hide();
		RTW_WIDGET("btndeletechar")->Hide();
		RTW_WIDGET("btnsetcharpwd")->Hide();
		RTW_WIDGET("btnchangecharpwd")->Hide();
		RTW_WIDGET("btndelcharpwd")->Hide();
		if (RTW_WIDGET("fmpwd")->IsVisible())
		{
			RTW_WIDGET("fmpwd")->Hide();
		}
	}

	RTW_WIDGET_T(CUiCheckButton, "fmckusername.fmusername1.btnusername1")->SetChecked(false);
	RTW_WIDGET_T(CUiCheckButton, "fmckusername.fmusername2.btnusername2")->SetChecked(false);
	RTW_WIDGET_T(CUiCheckButton, "fmckusername.fmusername3.btnusername3")->SetChecked(false);
	RTW_WIDGET_T(CUiCheckButton, "fmckusername.fmusername4.btnusername4")->SetChecked(false);
	for (i = 0; i < info.chatCount; i++)
	{
		if (m_mapSelectActor.find(info.users[i].id) != m_mapSelectActor.end())
		{
			pActor = m_mapSelectActor[info.users[i].id];
		}
		m_listSelectChar.push_back(info.users[i].id);
		m_listSelGcActor.push_back(pActor);
		if (pActor)
		{
			if (m_iCurSelectChar == -1)
			{
				RTW_WIDGET("btnenter")->Hide();
				RTW_WIDGET("btnrestore")->Hide();
				RTW_WIDGET("btndeletechar")->Hide();
				RTW_WIDGET("btnsetcharpwd")->Hide();
				RTW_WIDGET("btnchangecharpwd")->Hide();
				RTW_WIDGET("btndelcharpwd")->Hide();
				if (RTW_WIDGET("fmpwd")->IsVisible())
				{
					RTW_WIDGET("fmpwd")->Hide();
				}
			}
			else if (i == m_iCurSelectChar)
			{
				RTW_WIDGET("btnenter")->Hide();
				RTW_WIDGET("btnrestore")->Hide();
				RTW_WIDGET("btndeletechar")->Hide();
				RTW_WIDGET("btnsetcharpwd")->Hide();
				RTW_WIDGET("btnchangecharpwd")->Hide();
				RTW_WIDGET("btndelcharpwd")->Hide();
				if (RTW_WIDGET("fmpwd")->IsVisible())
				{
					RTW_WIDGET("fmpwd")->Hide();
				}
				pActor->GetGraph()->SetBaseColor(RtgVertex3(0.5f, 0.5f, 0.5f));

				char buf[1024];
				memset(buf, 0, 1024);
				rt2_sprintf(buf, "fmckusername.fmusername%d.btnusername%d", m_iCurSelectChar + 1, m_iCurSelectChar + 1);
				RTW_WIDGET_T(CUiCheckButton, buf)->SetChecked(true);
				if (info.users[m_iCurSelectChar].isFrozen)
				{
					RTW_WIDGET("btnrestore")->Show();
				}
				else
				{
					RTW_WIDGET("btnenter")->Show();
					RTW_WIDGET("btndeletechar")->Show();
					if (info.users[m_iCurSelectChar].hasCharPwd)
					{
						if (RTW_WIDGET("btnsetcharpwd")->IsVisible())
						{
							RTW_WIDGET("btnsetcharpwd")->Hide();
						}
						RTW_WIDGET("btnchangecharpwd")->Move(rectBtnSetChar, true);
						RTW_WIDGET("btndelcharpwd")->Move(rectBtnChangeChar, true);
						RTW_WIDGET("btnchangecharpwd")->Show();
						RTW_WIDGET("btndelcharpwd")->Show();
					}
					else
					{
						if (RTW_WIDGET("btnchangecharpwd")->IsVisible())
						{
							RTW_WIDGET("btnchangecharpwd")->Hide();
						}
						if (RTW_WIDGET("btndelcharpwd")->IsVisible())
						{
							RTW_WIDGET("btndelcharpwd")->Hide();
						}
						RTW_WIDGET("btnsetcharpwd")->Show();
					}
				}
				if (RTW_WIDGET("fmpwd")->IsVisible())
				{
					RTW_WIDGET("fmpwd")->Hide();
				}
			}
			else
			{
				pActor->GetGraph()->SetBaseColor(RtgVertex3(0.f, 0.f, 0.f));
			}
		}
	}
	if (m_pBody)
	{
		RTW_WIDGET("fmckusername.fmusername1.btnusername1.lblock1")->Hide();
		RTW_WIDGET("fmckusername.fmusername2.btnusername2.lblock2")->Hide();
		RTW_WIDGET("fmckusername.fmusername3.btnusername3.lblock3")->Hide();
		RTW_WIDGET("fmckusername.fmusername4.btnusername4.lblock4")->Hide();

		RtgMatrix12 _SlotMatrix;

		// Modified by Wayne Wong 2010-12-01
		/*
		if (m_pBody->GetBoneMat(&_SlotMatrix, "bno1") && m_listSelGcActor.size()>0 && m_listSelGcActor[0])
		{
			//设置人物按钮相应的信息
			char cTmp128[128];
			std::string name;
			SUserActor* pActor = g_TableUserActor.FindUserActor(info.users[0].attributes.metier, info.users[0].attributes.metierLevel);
			if (pActor)
			{
				name = pActor->MetierName.c_str();
			}
			RTW_WIDGET("fmckusername.fmusername1.btnusername1.lbmetier1")->SetText(name);
			name = info.users[0].name;
			RTW_WIDGET("fmckusername.fmusername1.btnusername1.lbusername1")->SetText(name);
			rt2_sprintf(cTmp128, "%d", info.users[0].attributes.level);

			RTW_WIDGET("fmckusername.fmusername1.btnusername1.lblev1")->SetText(cTmp128);
			if (info.users[0].hasCharPwd)
			{
				RTW_WIDGET("fmckusername.fmusername1.btnusername1.lblock1")->Show();
			}
			else
			{
				RTW_WIDGET("fmckusername.fmusername1.btnusername1.lblock1")->Hide();
			}

			int iCharIndex = GetCharIndexByActorID(info.users[0].attributes.actorID);
			vector<SCharImage> images;
			if (!g_TableCharImage.GetCharImage(s_userID[iCharIndex], images))
				return;

			std::string strImage = "ui_texture/";
			strImage += images[info.users[0].attributes.HeadImageID].imageGame;
			strImage += ".dds";

			RtwImage *pImage = g_workspace.getImageFactory()->createImage(strImage);
			pImage->SetBlend(true);
			RTW_WIDGET("fmckusername.fmusername1.lbuser1")->SetBackgroundImage(pImage);

			m_listSelGcActor[0]->mBaseActor.SetMatrix_Row(3, *_SlotMatrix.Row(3));
		}

		if (m_pBody->GetBoneMat(&_SlotMatrix, "bno2") && m_listSelGcActor.size()>1 && m_listSelGcActor[1])
		{
			//设置人物按钮相应的信息
			char cTmp128[128];
			std::string name;
			SUserActor* pActor = g_TableUserActor.FindUserActor(info.users[1].attributes.metier, info.users[1].attributes.metierLevel);
			if (pActor)
			{
				name = pActor->MetierName.c_str();
			}
			RTW_WIDGET("fmckusername.fmusername2.btnusername2.lbmetier2")->SetText(name);
			name = info.users[1].name;
			RTW_WIDGET("fmckusername.fmusername2.btnusername2.lbusername2")->SetText(name);
			rt2_sprintf(cTmp128, "%d", info.users[1].attributes.level);

			RTW_WIDGET("fmckusername.fmusername2.btnusername2.lblev2")->SetText(cTmp128);
			if (info.users[1].hasCharPwd)
			{
				RTW_WIDGET("fmckusername.fmusername2.btnusername2.lblock2")->Show();
			}
			else
			{
				RTW_WIDGET("fmckusername.fmusername2.btnusername2.lblock2")->Hide();
			}

			int iCharIndex = GetCharIndexByActorID(info.users[1].attributes.actorID);

			vector<SCharImage> images;
			if (!g_TableCharImage.GetCharImage(s_userID[iCharIndex], images))
				return;

			std::string strImage = "ui_texture/";
			strImage += images[info.users[1].attributes.HeadImageID].imageGame;
			strImage += ".dds";

			RtwImage *pImage = g_workspace.getImageFactory()->createImage(strImage);
			pImage->SetBlend(true);
			RTW_WIDGET("fmckusername.fmusername2.lbuser2")->SetBackgroundImage(pImage);

			m_listSelGcActor[1]->mBaseActor.SetMatrix_Row(3, *_SlotMatrix.Row(3));
		}

		if (m_pBody->GetBoneMat(&_SlotMatrix, "bno3") && m_listSelGcActor.size()>2 && m_listSelGcActor[2])
		{
			//设置人物按钮相应的信息
			char cTmp128[128];
			std::string name;
			SUserActor* pActor = g_TableUserActor.FindUserActor(info.users[2].attributes.metier, info.users[2].attributes.metierLevel);
			if (pActor)
			{
				name = pActor->MetierName.c_str();
			}
			RTW_WIDGET("fmckusername.fmusername3.btnusername3.lbmetier3")->SetText(name);
			name = info.users[2].name;
			RTW_WIDGET("fmckusername.fmusername3.btnusername3.lbusername3")->SetText(name);
			rt2_sprintf(cTmp128, "%d", info.users[2].attributes.level);

			RTW_WIDGET("fmckusername.fmusername3.btnusername3.lblev3")->SetText(cTmp128);
			if (info.users[2].hasCharPwd)
			{
				RTW_WIDGET("fmckusername.fmusername3.btnusername3.lblock3")->Show();
			}
			else
			{
				RTW_WIDGET("fmckusername.fmusername3.btnusername3.lblock3")->Hide();
			}

			int iCharIndex = GetCharIndexByActorID(info.users[2].attributes.actorID);;
			vector<SCharImage> images;
			if (!g_TableCharImage.GetCharImage(s_userID[iCharIndex], images))
				return;

			std::string strImage = "ui_texture/";
			strImage += images[info.users[2].attributes.HeadImageID].imageGame;
			strImage += ".dds";

			RtwImage *pImage = g_workspace.getImageFactory()->createImage(strImage);
			pImage->SetBlend(true);
			RTW_WIDGET("fmckusername.fmusername3.lbuser3")->SetBackgroundImage(pImage);

			m_listSelGcActor[2]->mBaseActor.SetMatrix_Row(3, *_SlotMatrix.Row(3));
		}


		if (m_pBody->GetBoneMat(&_SlotMatrix, "bno4") && m_listSelGcActor.size()>3 && m_listSelGcActor[3])
		{
			//设置人物按钮相应的信息
			char cTmp128[128];
			std::string name;
			SUserActor* pActor = g_TableUserActor.FindUserActor(info.users[3].attributes.metier, info.users[3].attributes.metierLevel);
			if (pActor)
			{
				name = pActor->MetierName.c_str();
			}
			RTW_WIDGET("fmckusername.fmusername4.btnusername4.lbmetier4")->SetText(name);
			name = info.users[3].name;
			RTW_WIDGET("fmckusername.fmusername4.btnusername4.lbusername4")->SetText(name);
			rt2_sprintf(cTmp128, "%d", info.users[3].attributes.level);

			RTW_WIDGET("fmckusername.fmusername4.btnusername4.lblev4")->SetText(cTmp128);
			if (info.users[3].hasCharPwd)
			{
				RTW_WIDGET("fmckusername.fmusername4.btnusername4.lblock4")->Show();
			}
			else
			{
				RTW_WIDGET("fmckusername.fmusername4.btnusername4.lblock4")->Hide();
			}

			int iCharIndex = GetCharIndexByActorID(info.users[3].attributes.actorID);;

			vector<SCharImage> images;
			if (!g_TableCharImage.GetCharImage(s_userID[iCharIndex], images))
				return;

			std::string strImage = "ui_texture/";
			strImage += images[info.users[3].attributes.HeadImageID].imageGame;
			strImage += ".dds";

			RtwImage *pImage = g_workspace.getImageFactory()->createImage(strImage);
			pImage->SetBlend(true);
			RTW_WIDGET("fmckusername.fmusername4.lbuser4")->SetBackgroundImage(pImage);

			m_listSelGcActor[3]->mBaseActor.SetMatrix_Row(3, *_SlotMatrix.Row(3));
		}
		*/

		size_t nSize = m_listSelGcActor.size();
		nSize = min(4, nSize);
		for (size_t i = 0; i < nSize; ++i)
		{
			char boneName[20];
			rt2_sprintf(boneName, "bno%u", i + 1);
			if (m_pBody->GetBoneMat(&_SlotMatrix, boneName) && m_listSelGcActor[i])
			{
				//设置人物按钮相应的信息
				std::string name;
				char cTmp128[128], widgetName[128];
				SUserActor* pActor = g_TableUserActor.FindUserActor(info.users[i].attributes.metier, info.users[i].attributes.metierLevel);
				if (pActor)
					name = pActor->MetierName.c_str();
				rt2_sprintf(widgetName, "fmckusername.fmusername%u.btnusername%u.lbmetier%u", i + 1, i + 1, i + 1);
				RTW_WIDGET(widgetName)->SetText(name);

				name = info.users[i].name;
				rt2_sprintf(widgetName, "fmckusername.fmusername%u.btnusername%u.lbusername%u", i + 1, i + 1, i + 1);
				RTW_WIDGET(widgetName)->SetText(name);

				rt2_sprintf(cTmp128, "%d", info.users[i].attributes.level);
				rt2_sprintf(widgetName, "fmckusername.fmusername%u.btnusername%u.lblev%u", i + 1, i + 1, i + 1);
				RTW_WIDGET(widgetName)->SetText(cTmp128);

				rt2_sprintf(widgetName, "fmckusername.fmusername%u.btnusername%u.lblock%u", i + 1, i + 1, i + 1);
				if (info.users[i].hasCharPwd)
					RTW_WIDGET(widgetName)->Show();
				else
					RTW_WIDGET(widgetName)->Hide();

				int iCharIndex = GetCharIndexByActorID(info.users[i].attributes.actorID);
				vector<SCharImage> images;
				if (!g_TableCharImage.GetCharImage(s_userID[iCharIndex], images))
					return;

				std::string strImage = "ui_texture/";
				strImage += images[info.users[i].attributes.HeadImageID].imageGame;
				strImage += ".dds";

				RtwImage* pImage = g_workspace.getImageFactory()->createImage(strImage);
				pImage->SetBlend(true);
				rt2_sprintf(widgetName, "fmckusername.fmusername%u.lbuser%u", i + 1, i + 1);
				RTW_WIDGET(widgetName)->SetBackgroundImage(pImage);

				m_listSelGcActor[i]->mBaseActor.SetMatrix_Row(3, *_SlotMatrix.Row(3));
			}

		}
	}

	//挂宝宝
	if (!m_pPetActor1)
	{
		const std::string szPetName = "mon502a";
		m_pPetActor1 = RtcGetActorManager()->CreateActor(szPetName.c_str(), true);
		if (m_pPetActor1 == NULL)
		{
			ERR1("[GcActor::Render] 无法载入武器模型，name=%s.\n", szPetName.c_str());
			return;
		}
		m_pPetActor1->LinkParent(m_pBody, "bb01");
	}
	if (!m_pPetActor2)
	{
		const std::string szPetName = "mon502a";
		m_pPetActor2 = RtcGetActorManager()->CreateActor(szPetName.c_str(), true);
		if (m_pPetActor2 == NULL)
		{
			ERR1("[GcActor::Render] 无法载入武器模型，name=%s.\n", szPetName.c_str());
			return;
		}
		m_pPetActor2->LinkParent(m_pBody, "bb02");
	}
	if (!m_pPetActor3)
	{
		const std::string szPetName = "mon144d";
		m_pPetActor3 = RtcGetActorManager()->CreateActor(szPetName.c_str(), true);
		if (m_pPetActor3 == NULL)
		{
			ERR1("[GcActor::Render] 无法载入武器模型，name=%s.\n", szPetName.c_str());
			return;
		}
		m_pPetActor3->LinkParent(m_pBody, "bb03");
	}

	for (i = 0; i < m_listSelGcActor.size(); i++)
	{
		pActor = m_listSelGcActor[i];
		if (pActor && pActor->m_pHUD)
		{
			pActor->SetName(info.users[i].name);
			pActor->m_pHUD->Hide();
			pActor->Run(0.01f);
		}
	}

	OnRun(0.f);
	unguard;
}

//进入角色创建
void GcLogin::EnterCreateChar()
{
	guard;

	UpdateGraphConfig("Graph_CreateChar");

	if (m_pCamera)
	{
		m_pCamera = NULL;
	}

	m_iCurSelectChar = -1;

	// 生成可选人物的列表
	int i, iUserIdx;
	iUserIdx = 0;
	const SUserActor* pUser;
	CRT_ActorInstance* pActor;
	std::map<std::string, CRT_ActorInstance*>::iterator it;

	if (m_mapCreateActor.size() == 0)
	{
		for (i = 0; i < g_TableUserActor.GetUserCount(); i++)
		{
			pUser = g_TableUserActor.GetUserByIdx(i);
			if (pUser)
			{
				if (!pUser->bZCreate && !pUser->bSCreate) continue;
				if (m_mapCreateActor.find(pUser->Id) == m_mapCreateActor.end())
				{
					iUserIdx = GetCharIndexByActorID(pUser->Id);
					pActor = FindModel(s_pszLoginCharName[iUserIdx]);
					if (pActor)
					{
						// Load Skin and Link Weapon
						pActor->LoadSkin(s_pszCharCreateBody[iUserIdx], false);
						pActor->LoadSkin(s_pszCharCreateHand[iUserIdx], false);
						pActor->LoadSkin(s_pszCharCreateLeg[iUserIdx], false);
						pActor->LoadSkin(s_pszCharCreateWaist[iUserIdx], false);

						vector<SHeadModel> heads;
						if (g_TableHeadModel.GetHeadModel(s_userID[iUserIdx], heads))
						{
							m_HeadModelIndex[iUserIdx] = 0;
							pActor->LoadSkin(heads[0].skin.c_str(), true);
						}
						if (pUser->Id == WU_TAI_ID) //if (iUserIdx == 0)
						{
							//创建五台武器
							if (!m_pWeaponWT)
							{
								const std::string mWTModelName = "w_wtj_m_06_04";
								m_pWeaponWT = RtcGetActorManager()->CreateActor(mWTModelName.c_str(), true);
								if (m_pWeaponWT == NULL)
								{
									ERR1("[GcActor::Render] 无法载入武器模型，name=%s.\n", mWTModelName.c_str());
									return;
								}
								m_pWeaponWT->LinkParent(pActor, "Box01");
							}
						}
						else if (pUser->Id == HUA_JIAN_ID) //else if (iUserIdx == 1)
						{
							//创建花间丝带
							if (!m_pWeaponHL)
							{
								const std::string mHJModelName = "w_hjl_f_07";
								m_pWeaponHL = RtcGetActorManager()->CreateActor(mHJModelName.c_str(), true);
								if (m_pWeaponHL == NULL)
								{
									ERR1("[GcActor::Render] 无法载入武器模型，name=%s.\n", mHJModelName.c_str());
									return;
								}
								m_pWeaponHL->LinkParent(pActor, "Box03");
							}
						}
						else if (pUser->Id == SHU_SHAN_ID) //else if(iUserIdx == 2)
						{
							//创建蜀山飞轮和轨迹动画
							if (!m_pWeaponFL)
							{
								const std::string mSSModelName = "w_ssl_m_04_03";
								m_pWeaponFL = RtcGetActorManager()->CreateActor(mSSModelName.c_str(), true);
								if (m_pWeaponFL == NULL)
								{
									ERR1("[GcActor::Render] 无法载入武器模型，name=%s.\n", mSSModelName.c_str());
									return;
								}
							}

							if (!m_pWeaponFLWay)
							{
								const std::string mWayName = "ss_fl";
								m_pWeaponFLWay = RtcGetActorManager()->CreateActor(mWayName.c_str(), true);
								if (m_pWeaponFLWay == NULL)
								{
									ERR1("[GcActor::Render] 无法载入武器模型，name=%s.\n", mWayName.c_str());
									return;
								}
							}
						}
						else if (pUser->Id == MIAO_JIANG_ID) //else if (iUserIdx == 3)
						{
							//创建苗疆武器
							if (!m_pWeaponMJ)
							{
								const std::string mMJModelName = "w_mjz_f_03_01";
								m_pWeaponMJ = RtcGetActorManager()->CreateActor(mMJModelName.c_str(), true);
								if (m_pWeaponMJ == NULL)
								{
									ERR1("[GcActor::Render] 无法载入武器模型，name=%s.\n", mMJModelName.c_str());
									return;
								}
								m_pWeaponMJ->LinkParent(pActor, "Box01");
							}
						}
					}

					m_mapCreateActor[pUser->Id] = pActor;
				}
			}
		}
	}

	for (i = 0; i < 6; i++)
	{
		it = m_mapActor.find(std::string(s_pszLoginCharName[i]));
		if (it != m_mapActor.end()) m_mapActor.erase(it);
	}

	m_listSelActor.clear();
	m_listSelectChar.clear();

	for (i = 0; i < g_TableUserActor.GetUserCount(); i++)
	{
		pUser = g_TableUserActor.GetUserByIdx(i);
		if (pUser)
		{
			if (!pUser->bZCreate && !pUser->bSCreate) continue;
			if (m_mapCreateActor.find(pUser->Id) != m_mapCreateActor.end())
			{
				pActor = m_mapCreateActor[pUser->Id];
			}
			m_listSelectChar.push_back(pUser->Id);
			m_listSelActor.push_back(pActor);
		}
	}
	if (m_listSelectChar.size() == 0)
	{
		ShowMessage(R(LMSG_CANNOT_CREATE_CHAR));
	}

	// Link 人物---bno1花间女-bno2蜀山男
	m_pBody = FindModel("Body");
	if (m_pBody)
	{
		RtgMatrix16 mActorMatrix;
		RtgMatrix12  WeaponMatrix;

		RtgMatrix12 _SlotMatrix;

		// Modified by Wayne Wong 2010-11-26 for Demo
		for (size_t index = 0; index < m_listSelectChar.size(); ++index)
		{
			short actorID = m_listSelectChar[index];
			if (actorID == WU_TAI_ID) //五台山
			{
				if (m_pBody->GetBoneMat(&_SlotMatrix, "bno8"))
				{
					mActorMatrix = _SlotMatrix;
					mActorMatrix.RotateLX(DegreeToRadian(90.f));
					mActorMatrix.RotateLZ(DegreeToRadian(180.f));
					m_listSelActor[index]->SetMatrix(mActorMatrix);
				}
			}
			else if (actorID == HUA_JIAN_ID) //花间门
			{
				if (m_pBody->GetBoneMat(&_SlotMatrix, "bno6"))
				{
					_AdjustCameraMatrix(&mActorMatrix, &_SlotMatrix);
					m_listSelActor[index]->SetMatrix(mActorMatrix);
				}
			}
			else if (actorID == SHU_SHAN_ID)	//蜀山派
			{
				if (m_pBody->GetBoneMat(&_SlotMatrix, "bno5"))
				{
					_AdjustCameraMatrix(&mActorMatrix, &_SlotMatrix);
					WeaponMatrix = *m_pWeaponFL->GetMatrix();
					WeaponMatrix = mActorMatrix;
					WeaponMatrix._32 += 25.f;
					m_pWeaponFLWay->SetMatrix(WeaponMatrix);
					m_pWeaponFLWay->LinkParent(m_listSelActor[index]);
					m_listSelActor[index]->SetMatrix(mActorMatrix);
				}
			}
			else if (actorID == MIAO_JIANG_ID)	//苗疆
			{
				if (m_pBody->GetBoneMat(&_SlotMatrix, "bno7"))
				{
					_AdjustCameraMatrix(&mActorMatrix, &_SlotMatrix);
					mActorMatrix = _SlotMatrix;
					mActorMatrix.RotateLX(DegreeToRadian(28.f));
					mActorMatrix.RotateLY(DegreeToRadian(10.f));
					mActorMatrix.RotateLZ(DegreeToRadian(-90.f));
					m_listSelActor[index]->SetMatrix(mActorMatrix);
				}
			}
		}
	}

	OnEnterCreateChar();
	unguard;
}

bool GcLogin::LeaveCreateChar()
{
	guard;
	CRT_ActorInstance* pActor;
	if (m_bCanInput)
	{
		CHECK(m_eStatus == GLS_CREATE_CHAR);
		if (m_iCurSelectChar < 0 || m_iCurSelectChar > 4)
		{
			pActor = NULL;
		}

		m_iCurSelectChar = -1;

		OnLeaveCreateChar();
	}

	m_listSelActor.clear();
	m_pCamera = NULL;
	g_pMusicThread->Stop();
	g_pMusicThread->Play("login_bg.ogg", true);
	return true;
	unguard;
}

void GcLogin::OnEnterCreateChar()
{
	UILayer::EnterSelectChar();
	m_bCanInput = true;

	// Modified By Wayne Wong 2010-11-26 for demo
	RTW_WIDGET("fmmingwang")->Show();
	RTW_WIDGET("fmshengwu")->Show();
	RTW_WIDGET("lbmiaojiangtext")->Show();
	RTW_WIDGET("lbwutaitext")->Show();

	RTW_WIDGET("fmhuajian")->Show();
	RTW_WIDGET("fmshushan")->Show();
	LOAD_UI("btnback")->Show();
	RTW_WIDGET("lbshushantext")->Show();
	RTW_WIDGET("lbhuajiantext")->Show();
	//进入创人状态,将选人状态UI隐藏
	if (RTW_WIDGET("fmckusername")->IsVisible())
	{
		RTW_WIDGET("fmckusername")->Hide();
	}
	if (RTW_WIDGET("btnenter")->IsVisible())
	{
		RTW_WIDGET("btnenter")->Hide();
	}
	if (RTW_WIDGET("btndeletechar")->IsVisible())
	{
		RTW_WIDGET("btndeletechar")->Hide();
	}
	if (RTW_WIDGET("btnsetcharpwd")->IsVisible())
	{
		RTW_WIDGET("btnsetcharpwd")->Hide();
	}
	if (RTW_WIDGET("btnchangecharpwd")->IsVisible())
	{
		RTW_WIDGET("btnchangecharpwd")->Hide();
	}
	if (RTW_WIDGET("btndelcharpwd")->IsVisible())
	{
		RTW_WIDGET("btndelcharpwd")->Hide();
	}
	if (RTW_WIDGET("btnexit")->IsVisible())
	{
		RTW_WIDGET("btnexit")->Hide();
	}
	if (RTW_WIDGET("fmpwd")->IsVisible())
	{
		RTW_WIDGET("fmpwd")->Hide();
	}
	if (RTW_WIDGET("btnbacklogin")->IsVisible())
	{
		RTW_WIDGET("btnbacklogin")->Hide();
	}
	//if (RTW_WIDGET("btnuserleft")->IsVisible())
	//{
	//	RTW_WIDGET("btnuserleft")->Hide();
	//}
	//if (RTW_WIDGET("btnuserright")->IsVisible())
	//{
	//	RTW_WIDGET("btnuserright")->Hide();
	//}
	m_pBody->RegisterNotify(this);
	OnRun(0.f);
}

void GcLogin::OnLeaveCreateChar()
{
	m_pBody->RegisterNotify(NULL);
	m_bCanInput = false;
	RTW_WIDGET("fmcreatid1")->Hide();
	RTW_WIDGET("fmcreatid2")->Hide();
	RTW_WIDGET("fmcreatid3")->Hide();
	RTW_WIDGET("fmmingwang")->Hide();
	RTW_WIDGET("fmhuajian")->Hide();
	RTW_WIDGET("fmshushan")->Hide();
	RTW_WIDGET("fmshengwu")->Hide();
	RTW_WIDGET("btnmale")->Hide();
	RTW_WIDGET("btnfemale")->Hide();
	LOAD_UI("btnback")->Hide();
	RTW_WIDGET("lbshushantext")->Hide();
	RTW_WIDGET("lbhuajiantext")->Hide();
	RTW_WIDGET("lbmiaojiangtext")->Hide();
	RTW_WIDGET("lbwutaitext")->Hide();
}

//loading页面加载
void GcLogin::EnterLoading()
{
	guard;
	m_bLoading = false;
	UILayer::EnterLoading();
	unguard;
}

bool GcLogin::LeaveLoading()
{
	guard;
	// 打开背景音乐
	const char* szMusicFileName = GetGameIni()->GetEntry("Audio", "LoginMusic");
	if (szMusicFileName)
	{
		g_pBackMusic->Play(szMusicFileName, true);
	}
	UILayer::LeaveLoading();

	return true;
	unguard;
}

void GcLogin::OnLoading()
{
	guard;
	const char* szLoginConfigName = "scene/login.ini";

	if (!m_ini.OpenFile(szLoginConfigName))
	{
		ERR1("错误: 不能登录界面配置文件[%s].\n", szLoginConfigName);
		return;
	}
	m_ini.CloseFile();

	LoadLoginSection(&m_ini, "Login", m_mapLogin);
	LoadLoginSection(&m_ini, "SelectChar", m_mapSelectChar);
	LoadLoginSection(&m_ini, "CreateChar", m_mapCreateChar);

	// 打开登陆界面
	UILayer::EnterLogin();
	unguard;
}

void GcLogin::UpdateGraphConfig(const char* szName)
{
	// 摄像机的视场角度（Field of View），单位为度
	float fCameraFOV = 45.0f;
	// 渲染视口的宽高比，例如4:3
	float fCameraAspect = 4.f / 3.f;
	//近截面
	float fCameraNear = 10.0f;
	//远截面
	float fCameraFar = 30000.0f;
	//雾效果开始的距离
	float fFogNear = 1000.f;
	//雾效果结束的距离
	float fFogFar = 8000.f;
	//雾效果开关
	long  lFogEnable = 0;
	//天空光的颜色，这里设置为深灰色
	long  lSkyLight = 0xFF888888;
	//主光源的红色分量强度
	float fLightR = 70.f;
	//主光源的宽度，0表示点光源
	float fLightW = 0.f;
	//辅助光源的红色分量强度
	float fLight2R = 70.f;
	//辅助光源的宽度，0表示点光源
	float fLight2W = 0.f;
	m_lSkyFog = false;
	RtgVertex3 vSkyLight(0.5f, 0.5f, 0.5f);
	RtgVertex3 vFogLight(0.5f, 0.5f, 0.5f);
	char* pBuf;

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
	if (pBuf)
	{
		sscanf(pBuf, "%f,%f,%f", &(vSkyLight.x), &(vSkyLight.y), &(vSkyLight.z));
		vSkyLight /= 255.f;
	}
	pBuf = (char*)m_ini.GetEntry(szName, "SunLight");
	if (pBuf)
	{
		sscanf(pBuf, "%f,%f,%f", &(m_lightDirect.vDiffuse.x), &(m_lightDirect.vDiffuse.y), &(m_lightDirect.vDiffuse.z));
		m_lightDirect.vDiffuse /= 255.f;
		m_lightDirect.vSpecular = m_lightDirect.vDiffuse;
	}
	pBuf = (char*)m_ini.GetEntry(szName, "SunLight2");
	if (pBuf)
	{
		sscanf(pBuf, "%f,%f,%f", &(m_lightDirect2.vDiffuse.x), &(m_lightDirect2.vDiffuse.y), &(m_lightDirect2.vDiffuse.z));
		m_lightDirect2.vDiffuse /= 255.f;
		m_lightDirect2.vSpecular = m_lightDirect2.vDiffuse;
		m_bLight2 = true;
	}
	else
	{
		m_bLight2 = false;
	}
	pBuf = (char*)m_ini.GetEntry(szName, "FogColor");
	if (pBuf)
	{
		sscanf(pBuf, "%f,%f,%f", &(vFogLight.x), &(vFogLight.y), &(vFogLight.z));
		vFogLight /= 255.f;
	}

	GetDevice()->m_pCamera->SetProjParams(DegreeToRadian(fCameraFOV), GetDevice()->m_pCamera->m_fAspect, fCameraNear, fCameraFar);
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

//人物角色左旋
void GcLogin::OnLeftRotation()
{
	m_listSelGcActor[m_iCurSelectChar]->GetGraph()->p()->RotateLZ(DegreeToRadian(30.f));

	/*
	guard;
	RtgMatrix12* pActorMatrix;
	pActorMatrix = m_listSelGcActor[m_iCurSelectChar]->GetGraph()->p()->GetMatrix();
	//RtgMatrix16* pActorMatrix;
	//pActorMatrix = m_listSelGcActor[m_iCurSelectChar]->mBaseActor.Matrix();
	pActorMatrix->RotateLZ(DegreeToRadian(30.f));
	//m_listSelGcActor[m_iCurSelectChar]->GetGraph()->SetRotation(*pActorMatrix);
	unguard;

	*/
}

static int mRotationTime = 0;
void GcLogin::OnLeftRotationDown()
{
	mRotationTime++;
	m_listSelGcActor[m_iCurSelectChar]->GetGraph()->p()->RotateLZ((mRotationTime * 30) % 360);

	/*
	RtgMatrix12* pActorMatrix;
	pActorMatrix = m_listSelGcActor[m_iCurSelectChar]->GetGraph()->p()->GetMatrix();
	pActorMatrix->RotateLZ(DegreeToRadian((mRotationTime*30)%360));
	*/
}
void GcLogin::OnLeftRotationUp()
{

}
//人物角色右旋
void GcLogin::OnRightRotation()
{
	m_listSelGcActor[m_iCurSelectChar]->GetGraph()->p()->RotateLZ(DegreeToRadian(-30.f));

	/*
		guard;
		RtgMatrix12* pActorMatrix;
		pActorMatrix = m_listSelGcActor[m_iCurSelectChar]->GetGraph()->p()->GetMatrix();
		//RtgMatrix16* pActorMatrix;
		//pActorMatrix = m_listSelGcActor[m_iCurSelectChar]->mBaseActor.Matrix();
		//pActorMatrix->_22 -= 30.f;
		pActorMatrix->RotateLZ(DegreeToRadian(-30.f));
		//m_listSelGcActor[m_iCurSelectChar]->GetGraph()->SetRotation(*pActorMatrix);
		unguard;
		*/
}
//进入游戏
void GcLogin::OnSelectUser()
{
	guard;
	if (m_iCurSelectChar >= 0 && m_iCurSelectChar <= 4)
	{
		if (GetAccountInfo().users[m_iCurSelectChar].hasCharPwd)
		{
			bSelectUserWithPwd = true;
			RTW_WIDGET("fmpwd.fmtxtpwd.txtnumpwd")->SetText("");
			RTW_WIDGET("fmpwd.lbtxt1")->SetText(std::string("请输入人物角色密码"));
			RTW_WIDGET("fmpwd")->Show();
		}
		else
		{
			UILayer::EnterLoading();
			SelectChar(GetAccountInfo().users[m_iCurSelectChar].id);
		}
	}
	else
	{
		ShowMessage(R(LMSG_PLS_CHOOSE_CHAR));
	}
	unguard;
}

void GcLogin::OnSelectUserWithPwd()
{
	if (RTW_WIDGET("fmpwd")->IsVisible())
	{
		RTW_WIDGET("fmpwd")->Hide();
	}
	UILayer::EnterLoading();
	SelectChar(GetAccountInfo().users[m_iCurSelectChar].id);
}

void GcLogin::OnRestoreChar()
{
	guard;
	if (m_iCurSelectChar >= 0 && m_iCurSelectChar <= 4)
	{
		RestoreChar(GetAccountInfo().users[m_iCurSelectChar].id);
	}
	else
	{
		ShowMessage(R(LMSG_PLS_CHOOSE_CHAR));
	}
	unguard;
}

void GcLogin::OnRestoreCharSuccess()
{
	UIFormMsg::ShowStatic(R(MSG_CHARACTER_RECOVEREDSUCCESS));
	QueryAccountInfo();
}
void GcLogin::OnDeleteUser()
{
	guard;
	if (m_iCurSelectChar >= 0 && m_iCurSelectChar < 4)
	{
		DeleteChar(GetAccountInfo().users[m_iCurSelectChar].id);
	}
	else
	{
		ShowMessage(R(LMSG_PLS_CHOOSE_CHAR));
	}
	unguard;
}
//设置角色密码
void GcLogin::OnSetCharPassword(const char* password)
{
	guard;
	if (m_iCurSelectChar >= 0 && m_iCurSelectChar < 4)
	{
		SetCharPassword(GetAccountInfo().users[m_iCurSelectChar].id, password);
	}
	unguard;
}
//变更角色密码
void GcLogin::OnChangeCharPassword(const char* old, const char* newpassword)
{
	guard;
	if (m_iCurSelectChar >= 0 && m_iCurSelectChar < 4)
	{
		ChangeCharPassword(GetAccountInfo().users[m_iCurSelectChar].id, old, newpassword);
	}
	unguard;
}
//解除密码锁
void GcLogin::OnDelCharPassword(const char* password)
{
	guard;
	if (m_iCurSelectChar >= 0 && m_iCurSelectChar < 4)
	{
		DelCharPassword(GetAccountInfo().users[m_iCurSelectChar].id, password);
	}
	unguard;
}

void GcLogin::OnCharPasswordConfirm(const char* password)
{
	guard;
	if (m_iCurSelectChar >= 0 && m_iCurSelectChar < 4)
	{
		CharPasswordConfirm(GetAccountInfo().users[m_iCurSelectChar].id, password);
	}
	unguard;
}

void GcLogin::ResetButtonPos()
{
	if (RTW_WIDGET("btnsetcharpwd")->IsVisible())
	{
		RTW_WIDGET("btnsetcharpwd")->Hide();
	}
	RTW_WIDGET("btnchangecharpwd")->Show();
	RTW_WIDGET("btndelcharpwd")->Show();
}

void GcLogin::OnNetDeleteUser(long id, char hasDel)
{
	if (hasDel)
	{
		std::map<DWORD, GcActor*>::iterator it = m_mapSelectActor.find(id);
		if (it != m_mapSelectActor.end())
		{
			DEL_ONE((*it).second);
			m_mapSelectActor.erase(it);
		}
	}
	QueryAccountInfo();
}

void GcLogin::OnRandomCreateUser()
{
	guard;
	CHECK(m_eStatus == GLS_CREATE_CHAR);
	//随机创建标志
	bRandom = true;

	int iCharIndex = GetCharIndexByActorID(m_listSelectChar[m_iCurSelectChar]);

	//显示随机外形
	RtwImage* pImage;
	pImage = g_workspace.getImageFactory()->createImage(s_pszCharAnimalName[iRandomAnimalIndex]);
	pImage->SetBlend(true);
	RTW_WIDGET("fmcreatid1.fmanimal.lbanimal")->SetBackgroundImage(pImage);

	vector<SHeadModel> heads;
	if (!g_TableHeadModel.GetHeadModel(s_userID[iCharIndex], heads))
		return;

	int size = heads.size();
	m_eNextHeadID = rand() % (heads.size());
	if (m_ePrevHeadID != m_eNextHeadID)
	{
		m_listSelActor[m_iCurSelectChar]->UnloadSkin(heads[m_ePrevHeadID].skin.c_str());
		m_listSelActor[m_iCurSelectChar]->LoadSkin(heads[m_eNextHeadID].skin.c_str(), true);
		RTW_WIDGET("fmcreatid2.fmhair.lbhairname")->SetText(heads[m_eNextHeadID].name);
		//RTW_WIDGET("fmcreatid2.fmface.lbfacename")->SetText(heads[m_HeadModelIndex[iCharIndex]].name);
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
	RTW_WIDGET("fmcreatid2.fmhead.lbpichead")->SetBackgroundImage(pImage);

	unguard;
}
void GcLogin::OnCreateUser()
{
	guard;
	CHECK(m_eStatus == GLS_CREATE_CHAR);

	if (m_iCurSelectChar == -1)
	{
		ShowMessage(R(LMSG_PLS_CHOOSE_CHAR));
	}
	if (RTW_WIDGET("fmcreatid1.fmname.txtname")->GetText().length() < 4 || RTW_WIDGET("fmcreatid1.fmname.txtname")->GetText().length() > 14)
	{
		ShowMessage(R(MSG_USERNAME_INVALID));
		return;
	}

	if (!RTW_WIDGET_T(CUiCheckButton, "btnmale")->GetChecked() && !RTW_WIDGET_T(CUiCheckButton, "btnfemale")->GetChecked())
	{
		ShowMessage(R(MSG_CHARACTER_SELECTSEXUAL));
		return;
	}
	std::string sUsername = RTW_WIDGET("fmcreatid1.fmname.txtname")->GetText().c_str();
	const char* szUsername = sUsername.c_str();
	if (m_iCurSelectChar < 0) return;
	char cActorID = (char)m_listSelectChar[m_iCurSelectChar];

	int iCharIndex = GetCharIndexByActorID(m_listSelectChar[m_iCurSelectChar]);

	vector<SHeadModel> heads;
	if (g_TableHeadModel.GetHeadModel(s_userID[iCharIndex], heads))
	{
		if (m_HeadModelIndex[iCharIndex] < (int)heads.size())
		{
			if (bRandom)
			{
				headRandomModelID = heads[m_ePrevHeadID].id;
			}
			else
			{
				headModelID = heads[m_HeadModelIndex[iCharIndex]].id;
			}
		}
	}
	if (bRandom)
	{
		this->CreateChar(szUsername, cActorID, FACTION_NEUTRAL, m_bSex, headRandomModelID, m_strCharPassword.c_str(), iRandomAnimalIndex, headRandomImageID);
	}
	else
	{
		this->CreateChar(szUsername, cActorID, FACTION_NEUTRAL, m_bSex, headModelID, m_strCharPassword.c_str(), iAnimalIndex, headImageID);

	}
	unguard;
}

void GcLogin::SetLastSelectCharID(int iID)
{
	//m_iLastSelectCharID = iID;
}

//lyymark 2.GcLogin.OnRun GcLogin帧主循环入口
void GcLogin::OnRun(float fSecond)
{

	guard;
	//lyytodo 锁帧 改为引擎graph主循环入口里面统一设定
	//CLockFrame lockFrame(1000.0 / 60);
	int i, j;
	RtgVertex3 vOrig, vDir, v0, v1, vMin, vMax;
	float r1, r2;
	RtgAABB* pAABB;
	POINT pt;
	RtgMatrix16 m16;
	static float fDiffSecond = 0.f;
	CRT_ActorInstance* pActor;

	//lyymark 2.GcLogin 更新声音
	g_pSoundMgr->UpdateAll(NULL, GetDevice()->GetAppTime());


	for (auto& pair : m_mapActor)
	{
		auto& act = pair.second;
		if (act)
		{
			act->Tick(fSecond * 1000.f, false);
		}
	}

	if (m_pWeaponFL)
	{
		m_pWeaponFL->Tick(fSecond * 1000.f, false);
	}
	if (m_pWeaponFLWay)
	{
		m_pWeaponFLWay->Tick(fSecond * 1000.f, false);
	}
	if (m_pPetActor1)
	{
		m_pPetActor1->Tick(fSecond * 1000.f, false);
	}
	if (m_pPetActor2)
	{
		m_pPetActor2->Tick(fSecond * 1000.f, false);
	}
	if (m_pPetActor3)
	{
		m_pPetActor3->Tick(fSecond * 1000.f, false);
	}

	if (m_eStatus == GLS_SELECT_GAMEWORLD_SERVER)
	{
		// 向导的网络
		OnGuideNetProcess(fSecond);
	}
	else
	{
		// 登陆的网络
		this->Process();
	}

	fDiffSecond += fSecond;
	//更新Camera
	if (m_eStatus == GLS_SELECT_GAMEWORLD_SERVER || m_eStatus == GLS_LOGIN || m_eStatus == GLS_SELECT_CHAR || m_eStatus == GLS_CREATE_CHAR)
	{
		if (m_pCamera)
		{
			RtgMatrix12 _SlotMatrix;
			//如果Camera有Bcam 则播放动画（移动镜头的效果）
			if (m_pCamera->GetBoneMat(&_SlotMatrix, "Bcam"))
			{
				RtgMatrix16 camMat;
				_AdjustCameraMatrix(&camMat, &_SlotMatrix);
				GetDevice()->m_pCamera->SetMatrix(camMat);
			}
		}
	}
	//更新飞轮动画
	RtgMatrix12 tmp;
	if (m_eStatus == GLS_CREATE_CHAR)
	{
		// Modified by Wayne Wong 2010-11-26 for demo
		for (size_t index = 0; index < m_listSelectChar.size(); ++index)
		{
			if (m_listSelectChar[index] == SHU_SHAN_ID)
			{
				if (m_pWeaponFL && m_pWeaponFLWay)
				{
					if (!m_pWeaponFLWay->GetBoneMat(&tmp, "Blink0"))
						tmp.Unit();
					tmp = tmp * (*m_listSelActor[index]->GetWorldMatrix());
					mWeaponMatrix = tmp;
					m_pWeaponFL->m_localMat = mWeaponMatrix;
					m_pWeaponFL->SetMatrix(mWeaponMatrix);
				}
				break;
			}
		}
	}
	if (m_eStatus == GLS_SELECT_CHAR || m_eStatus == GLS_CREATE_CHAR)
	{
		if (m_pPetActor1 && m_pPetActor1->IsPlayingPose())
		{
			m_pPetActor1->PlayPose("wait", false);
		}
		if (m_pPetActor2 && m_pPetActor2->IsPlayingPose())
		{
			m_pPetActor2->PlayPose("wait", false);
		}
		if (m_pPetActor3 && m_pPetActor3->IsPlayingPose())
		{
			m_pPetActor3->PlayPose("wait", false);
		}
		// update pose
		for (i = 0; i < m_listSelActor.size(); i++)
		{
			pActor = m_listSelActor[i];
			if (pActor)
			{
				if (!pActor->IsPlayingPose())
				{
					// Modified by Wayne Wong 2010-11-26 for Demo
					//int charIndex = GetCharIndexByActorID(m_listSelectChar[i]);
					int actorID = m_listSelectChar[i];
					if (actorID == WU_TAI_ID) // (charIndex == 0)
					{
						m_pWeaponWT->PlayPose("login_wait_n0", false);
						pActor->PlayPose("login_wait_n0", false);
					}
					else if (actorID == HUA_JIAN_ID) // (charIndex == 1)
					{
						m_pWeaponHL->PlayPose("wait_s0", false);
						pActor->PlayPose("wait_s0", false);
					}
					else if (actorID == SHU_SHAN_ID) // (charIndex == 2)
					{
						m_pWeaponFLWay->PlayPose("wait_l0", false);
						pActor->PlayPose("wait_l0", false);
					}
					else if (actorID == MIAO_JIANG_ID) // (charIndex == 3)
					{
						m_pWeaponMJ->PlayPose("login_wait_n0", false);
						pActor->PlayPose("login_wait_n0", false);
					}
				}

				pActor->Tick(fSecond * 1000.f);
			}
		}
		for (i = 0; i < m_listSelGcActor.size(); i++)
		{
			if (m_listSelGcActor[i])
			{
				if (m_listSelGcActor[i]->GetGraph() && m_listSelGcActor[i]->GetGraph()->p())
				{
					if (!m_listSelGcActor[i]->GetGraph()->p()->IsPlayingPose())
					{
						if (m_bCharFrozen[i] == 1)
						{
							m_listSelGcActor[i]->mBaseActor.PlayPose("siting_n0", false);
						}
						else
						{
							m_listSelGcActor[i]->mBaseActor.PlayPose(GcBaseActor::POSE_STAND);
						}


						//if (i == 0)
						//{
						//	m_listSelGcActor[0]->GetGraph()->p()->PlayPose("login_wait_n0", true);
						//}
						//else if (i == 1)
						//{
						//	m_listSelGcActor[1]->GetGraph()->p()->PlayPose("wait_s0",true);
						//} 
						//else if(i == 2)
						//{
						//	m_listSelGcActor[2]->GetGraph()->p()->PlayPose("wait_l0",true);
						//}
						//else if(i == 3)
						//{
						//	m_listSelGcActor[3]->GetGraph()->p()->PlayPose("login_wait_n0", true);
						//}
					}
				}
				m_listSelGcActor[i]->Run(fSecond);
			}
		}

		if (fDiffSecond > 0.1f)
		{
			fDiffSecond = 0.0f;
			if (!g_workspace.IsMouseMonopolist() && m_bCanInput)
			{
				GetCursorPos(&pt);
				ScreenToClient((HWND)GetApp()->GetWndHandle(), &pt);
				if (pt.x > 0 && pt.x < GetDevice()->m_iWndWidth && pt.y>0 && pt.y < GetDevice()->m_iWndHeight && GetDevice()->HasFocus())
				{
					GetDevice()->GetPickRayFromScreen(pt.x, pt.y, vOrig, vDir);
					if (m_listSelActor.size())
					{
						for (i = 0; i < m_listSelActor.size(); i++)
						{
							if (m_listSelActor[i] == NULL) continue;
							m16.Set4X3Matrix(*(m_listSelActor[i])->GetWorldMatrix());
							m16.Invert();
							v0 = vOrig;
							v1 = vOrig + vDir * 1000.f; // 只判断1000距离远的物体
							v0 = v0 * m16;
							v1 = v1 * m16;

							pAABB = (m_listSelActor[i])->GetStaticBoundingBox();
							if (pAABB == NULL) continue;

							vMin = pAABB->Min();
							vMax = pAABB->Max();
							if (rtgIsLineIntersectAABB(v0, v1, vMin, vMax, &r1, &r2))
							{
								break;
							}
						}
						v0.Set(0.f);
						v1.Set(1.f);
						for (j = 0; j < m_listSelActor.size(); j++)
						{
							if (m_listSelActor[j] == NULL) continue;
							if (i != j)
							{
								m_listSelActor[j]->SetBaseColor(v0, true);
							}
							else
							{
								m_listSelActor[j]->SetBaseColor(v1, true);
							}
						}
						if (m_iCurSelectChar >= 0 && m_iCurSelectChar != i)
						{
							if (m_listSelActor.size() > m_iCurSelectChar && m_listSelActor[m_iCurSelectChar])
							{
								m_listSelActor[m_iCurSelectChar]->SetBaseColor(RtgVertex3(0.5f, 0.5f, 0.5f), true);
							}
						}
					}

					if (m_listSelGcActor.size())
					{
						for (i = 0; i < m_listSelGcActor.size(); i++)
						{
							if (m_listSelGcActor[i] == NULL) continue;
							m16.Set4X3Matrix(*(m_listSelGcActor[i])->GetGraph()->p()->GetWorldMatrix());
							m16.Invert();
							v0 = vOrig;
							v1 = vOrig + vDir * 1000.f; // 只判断1000距离远的物体
							v0 = v0 * m16;
							v1 = v1 * m16;

							pAABB = (m_listSelGcActor[i])->GetGraph()->p()->GetStaticBoundingBox();
							if (pAABB == NULL) continue;

							vMin = pAABB->Min();
							vMax = pAABB->Max();
							if (rtgIsLineIntersectAABB(v0, v1, vMin, vMax, &r1, &r2))
							{
								break;
							}
						}
						v0.Set(0.f);
						v1.Set(1.f);
						for (j = 0; j < m_listSelGcActor.size(); j++)
						{
							if (m_listSelGcActor[j] == NULL) continue;
							if (i != j)
							{
								m_listSelGcActor[j]->GetGraph()->SetBaseColor(v0);
							}
							else
							{
								m_listSelGcActor[i]->GetGraph()->SetBaseColor(v1);
							}
						}
						if (m_iCurSelectChar >= 0 && m_iCurSelectChar != i)
						{
							if (m_listSelGcActor.size() > m_iCurSelectChar && m_listSelGcActor[m_iCurSelectChar])
							{
								m_listSelGcActor[m_iCurSelectChar]->GetGraph()->SetBaseColor(RtgVertex3(0.5f, 0.5f, 0.5f));
							}
						}
					}
				}
			}
		}
	}
	g_workspace.Run(fSecond * 1000);
	unguard;
}

void GcLogin::OnBeginRender()
{
	guard;
	unguard;
}

//lyymark 2.GcLogin.OnRun OnEndRender 渲染结束 打印帧率统计信息
void GcLogin::OnEndRender()
{
	guard;
	// 获取帧统计信息
	const TCHAR* frameStats = GetDevice()->GetFrameStats();

	// 获取鼠标位置
	int mouseX = g_workspace.GetMousePosX();
	int mouseY = g_workspace.GetMousePosY();

	// 格式化字符串
	TCHAR cTmp[128] = _T("\0");
	_stprintf_s(cTmp, _T("%s\r\nMouseX: %d\r\nMouseY: %d"), frameStats, mouseX, mouseY);

	// 绘制字符串
	GetDevice()->DrawString(0, 0, 0xFF00FF00, cTmp);
	unguard;
}

void GcLogin::OnRender(float fSecond)
{
	guard;
	GameSetCursor();

	GetDevice()->SetLight(0, &m_lightDirect);
	if (m_bLight2)
	{
		GetDevice()->SetLight(1, &m_lightDirect2);
		GetDevice()->SetLight(2, NULL);
	}
	else
	{
		GetDevice()->SetLight(1, NULL);
	}

	if (m_eStatus == GLS_LOADING)
	{
		if (m_bLoading)
		{
			OnLoading();
			SetLoginState(GLS_SELECT_GAMEWORLD_SERVER);
		}
		else
		{
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

void GcLogin::OnRenderMask(RTGRenderMask mask, float fSecond)
{
	guard;
	CRT_ActorInstance* pActor;
	std::map<std::string, CRT_ActorInstance*>::iterator it;

	RtgCamera* pCamera = GetDevice()->m_pCamera;


	for (const auto& pair : m_mapActor)
	{
		// 获取指向 Actor 实例的指针
		pActor = pair.second;

		// 优化渲染逻辑
		if (pCamera && !pCamera->CheckVisable(pActor->GetStaticBoundingBox()))
			continue;

		bool bFogChange = false;
		// 判断是否需要更改雾效
		if (!m_lSkyFog && pair.first == "Sky")
		{
			bFogChange = true;
		}

		// 如果需要更改雾效，调整设备设置
		if (bFogChange)
		{
			if (GetDevice()->GetFogEnable())
			{
				GetDevice()->SetFogEnable(FALSE);
			}
			else
			{
				bFogChange = false;
			}
		}

		// 渲染 Actor
		pActor->Render(GetDevice(), mask);

		// 恢复雾效设置
		if (bFogChange)
		{
			GetDevice()->SetFogEnable(TRUE);
		}
	}
	

	if (m_eStatus == GLS_SELECT_CHAR || m_eStatus == GLS_CREATE_CHAR)
	{
		for (int i = 0; i < m_listSelActor.size(); i++)
		{
			pActor = m_listSelActor[i];
			if (pActor)
			{
				pActor->Render(GetDevice(), mask, true, true);
			}
		}
		for (int i = 0; i < m_listSelGcActor.size(); i++)
		{
			if (m_listSelGcActor[i])
			{
				if (m_bCharFrozen[i])
				{
					m_listSelGcActor[i]->GetGraph()->SetVisible(0.3f);
				}
				m_listSelGcActor[i]->Render(*GetDevice(), mask);
			}
		}
	}
	if (m_eStatus == GLS_CREATE_CHAR)
	{
		if (m_pWeaponWT)
		{
			m_pWeaponWT->Render(GetDevice(), mask);
		}
		if (m_pWeaponFL)
		{
			m_pWeaponFL->Render(GetDevice(), mask);
		}
		if (m_pWeaponFLWay)
		{
			m_pWeaponFLWay->Render(GetDevice(), mask);
		}
		if (m_pWeaponHL)
		{
			m_pWeaponHL->Render(GetDevice(), mask);
		}
		if (m_pWeaponMJ)
		{
			m_pWeaponMJ->Render(GetDevice(), mask);
		}
	}
	if (m_eStatus == GLS_SELECT_CHAR)
	{
		if (m_pPetActor1)
		{
			m_pPetActor1->Render(GetDevice(), mask);
		}
		if (m_pPetActor2)
		{
			m_pPetActor2->Render(GetDevice(), mask);
		}
		if (m_pPetActor3)
		{
			m_pPetActor3->Render(GetDevice(), mask);
		}
	}
	unguard;
}

void GcLogin::OnMouseMove(int iButton, int x, int y, int increaseX, int increaseY)
{
	guard;
	if (!m_bCanInput) return;
	unguard;
}
void GcLogin::OnMouseWheel(int iButton, long vDelta)
{
	guard;
#ifdef _DEBUG
	GetDevice()->m_pCamera->MoveForward(vDelta * 100.0f);
#endif
#ifdef _PREVIEW
	GetDevice()->m_pCamera->MoveForward(vDelta * 100.0f);
#endif
	if (!m_bCanInput) return;
	unguard;
}

void GcLogin::OnMouseLDown(int iButton, int x, int y)
{
	guard;
	if (!m_bCanInput) return;
	if (g_workspace.IsMouseMonopolist()) return;
	int i = 0;
	RtgMatrix16 m16;
	RtgVertex3 vOrig, vDir, v0, v1, vMin, vMax;
	RtgAABB* pAABB;
	float r1, r2;

	GetDevice()->GetPickRayFromScreen(x, y, vOrig, vDir);

	switch (m_eStatus)
	{
	case GLS_SELECT_CHAR:
		for (i = 0; i < m_listSelGcActor.size(); i++)
		{
			if (m_listSelGcActor[i] == NULL) continue;
			m16.Set4X3Matrix(*(m_listSelGcActor[i])->GetGraph()->p()->GetWorldMatrix());
			m16.Invert();
			v0 = vOrig;
			v1 = vOrig + vDir * 1000.f; // 只判断1000距离远的物体
			pAABB = (m_listSelGcActor[i])->GetGraph()->p()->GetStaticBoundingBox();
			if (pAABB == NULL) continue;
			v0 = v0 * m16;
			v1 = v1 * m16;
			vMin = pAABB->Min();
			vMax = pAABB->Max();
			if (rtgIsLineIntersectAABB(v0, v1, vMin, vMax, &r1, &r2))
			{
				SetSelectUser(i);
				break;
			}
		}
		break;
	case GLS_CREATE_CHAR:
		for (i = 0; i < m_listSelActor.size(); i++)
		{
			if (m_listSelActor[i] == NULL) continue;
			m16.Set4X3Matrix(*(m_listSelActor[i])->GetWorldMatrix());
			m16.Invert();
			v0 = vOrig;
			v1 = vOrig + vDir * 1000.f; // 只判断1000距离远的物体

			pAABB = (m_listSelActor[i])->GetStaticBoundingBox();
			if (pAABB == NULL) continue;

			v0 = v0 * m16;
			v1 = v1 * m16;
			vMin = pAABB->Min();
			vMax = pAABB->Max();
			if (rtgIsLineIntersectAABB(v0, v1, vMin, vMax, &r1, &r2))
			{
				break;
			}
		}
		if (i == m_listSelActor.size()) return;
		//点中后播放人物动画
		if (m_iCurSelectChar == 0)
		{
			const SRT_Pose* _pose = &m_listSelActor[0]->GetCurrentPose();

			if (_pose && _pose->Name != "login_f0")
			{
				if (m_pWeaponWT)
				{
					m_pWeaponWT->PlayPose("login_f0", false);
				}
				if (m_listSelActor[0])
				{
					m_listSelActor[0]->PlayPose("login_f0", false);
				}
			}
		}
		else if (m_iCurSelectChar == 1)
		{
			const SRT_Pose* _pose = &m_listSelActor[1]->GetCurrentPose();

			if (_pose && _pose->Name != "login_s0")
			{
				if (m_pWeaponHL)
				{
					m_pWeaponHL->PlayPose("login_s0", false);
				}
				if (m_listSelActor[1])
				{
					m_listSelActor[1]->PlayPose("login_s0", false);
				}
			}
		}
		else if (m_iCurSelectChar == 2)
		{
			const SRT_Pose* _pose = &m_listSelActor[2]->GetCurrentPose();

			if (_pose && _pose->Name != "login_l0")
			{
				m_pWeaponFLWay->PlayPose("login_l0", false);
				m_listSelActor[2]->PlayPose("login_l0", false);
			}
		}
		else if (m_iCurSelectChar == 3)
		{
			const SRT_Pose* _pose = &m_listSelActor[3]->GetCurrentPose();

			if (_pose && _pose->Name != "login_z0")
			{
				if (m_pWeaponMJ)
				{
					m_pWeaponMJ->PlayPose("login_z0", false);
				}
				if (m_listSelActor[3])
				{
					m_listSelActor[3]->PlayPose("login_z0", false);
				}
			}

		}
		break;
	}

	unguard;
}

void GcLogin::OnMouseLDClick(int iButton, int x, int y)
{
	guard;
	unguard;
}

void GcLogin::OnMouseRDown(int iButton, int x, int y)
{
	guard;
	if (!m_bCanInput) return;
	unguard;
}

void GcLogin::OnMouseRDrag(int iButton, int x, int y, int increaseX, int increaseY)
{
	guard;
#ifdef _DEBUG
	GetDevice()->m_pCamera->AddYaw(DegreeToRadian(increaseX));
	GetDevice()->m_pCamera->AddRoll(DegreeToRadian(-increaseY));
#endif
#ifdef _PREVIEW

	GetDevice()->m_pCamera->AddYaw(DegreeToRadian(increaseX));
	GetDevice()->m_pCamera->AddRoll(DegreeToRadian(-increaseY));
#endif
	if (m_eStatus == GLS_CREATE_CHAR)
	{
		if (m_iCurSelectChar == 0 || m_iCurSelectChar == 1)
		{
			m_listSelActor[m_iCurSelectChar]->RotateLZ(DegreeToRadian(increaseX / 5));
			/*RtgMatrix12* pActorMatrix;
			pActorMatrix = m_listSelActor[m_iCurSelectChar]->GetMatrix();
			if (pActorMatrix)
			{
				pActorMatrix->RotateLZ();
			}*/
		}
	}
	if (!m_bCanInput) return;
	unguard;
}

void GcLogin::OnMouseMDrag(int iButton, int x, int y, int increaseX, int increaseY)
{
	guard;
	if (!m_bCanInput) return;
	unguard;
}

void GcLogin::OnKeyDown(int iButton, int iKey)
{
	guard;
	//lyymark 2.GcLogin 按v显示当前ViewMatrix
#ifdef _PREVIEW
	if (iButton == 86) {
		auto gvm = GetDevice()->m_pCamera->GetViewMatrix();
		P_LOGINFO("ViewMatrix: \n" + gvm.ToString());
	}
#else
	if (!m_bCanInput) return;
#endif
	unguard;
	}

void GcLogin::OnKeyUp(int iButton, int iKey) {
	guard;
	if (!m_bCanInput) return;
	unguard;
}

void GcLogin::SelectGameWorld(int iIdx)
{
	guard;
	if (iIdx >= 0 && iIdx < ms_lGameWorldServerCount)
	{
		m_szGameWorldServerName = ms_pGameWorldServerList[iIdx].szName;
		m_szGameWorldServerIP = ms_pGameWorldServerList[iIdx].szIP;
		m_lGameWorldServerPort = ms_pGameWorldServerList[iIdx].lPort;
		SetLoginState(GLS_LOGIN);
	}
	unguard;
}

void GcLogin::Login(char* szUsername, char* szPassword)
{
	guard;
	m_szAccountUsername = szUsername;
	m_szAccountPassword = szPassword;

	if (m_szGameWorldServerIP.IsEmpty())
	{
		ShowMessage(R(LMSG_CANNOT_FIND_GWS_IP));
	}
	if (!this->ConnectServer(m_szGameWorldServerIP, m_lGameWorldServerPort))
	{
		LoginErrMsg(GLEM_CANNOT_CONNECT);
	}
	unguard;
}

void GcLogin::NetClose()
{
	guard;
	Close();
	unguard;
}

void GcLogin::OnNetLogin(int result, const char* szRetStr, short sRetCode, char cPointType, long lPoint)
{
	guard;
	LOG2("网络: %s [%s].\n", R(G_LOGIN), result == LOGIN_RET_SUCCESS ? R(G_SUCCEED) : R(G_FAIL));

	if (!this->QueryAccountInfo())
	{
		LoginErrMsg(GLEM_GET_ACCOUNT_FAIL);
	}

	if (result == LOGIN_RET_SUCCESS)
	{
		m_szAccountPassword = "";
		//tim.yang   pw登陆
		m_hallName.clear();
		m_hallKey.clear();
		m_ishallLogin = false;
	}
	else
	{
		EErrMsg err;

		switch (result)
		{
		default:								err = GLEM_LOGIN_FAIL;					break;
		case LOGIN_RET_FAILED_UNKNOWN:			err = GLEM_LOGIN_FAIL;					break;     //登录失败
		case LOGIN_RET_FAILED_USER_ONLINE:		err = GLEM_LOGIN_FAIL_USER_ONLINE;		break;     //此帐号正在被使用
		case LOGIN_RET_FAILED_ACCOUNT_STOP:		err = GLEM_LOGIN_FAIL_ACCOUNT_STOP;		break;     //此帐号被封停
		case LOGIN_RET_FAILED_ACCOUNT_PAUSE:	err = GLEM_LOGIN_FAIL_ACCOUNT_PAUSE;	break;     //密码错误
		case LOGIN_RET_FAILED_ACCOUNT_WRONG5:   err = GLEM_LOGIN_FAIL_ACCOUNT_WRONG5;   break;     //由于连续输入帐号密码错误，为保障用户信息安全，锁定帐号10分钟
		case LOGIN_RET_FAILED_POT_NOT_ENOUGH:	err = GLEM_LOGIN_FAIL_POT_NOT_ENOUGH;	break;     //帐号点数不够
		case LOGIN_RET_FAILED_PWD_WRONG:		err = GLEM_LOGIN_FAIL_PWD_WRONG;		break;     //登录失败,请检查您的密码和验证码是否正确
		case LOGIN_RET_FAILED_NOT_FOUND:		err = GLEM_LOGIN_FAIL_NOT_FOUND;		break;     //登录失败,请去网站激活该帐号
		case LOGIN_RET_FAILED_SERVER_FULL:      err = GLEM_LOGIN_FAIL_SERVER_FULL;		break;      //服务器已满
		case LOGIN_RET_FAILED_SERVER_STOP:      err = GLEM_LOGIN_FAIL_SERVER_STOP;		break;      //服务器维护中
		case LOGIN_RET_FAILED_NEW_CARD:         err = GLEM_LOGIN_FAIL_NEW_CARD;		    break;
		case LOGIN_RET_FAILED_PUBWIN_CENTER:	err = GLEM_LOGIN_FAIL_PUBWIN_CENTER;	break;      //同步会员中心失败
		case LOGIN_RET_ACCOUNT_IS_LOCKER:		err = GLEM_LOGIN_FAIL_IS_LOCKED;		break;      //帐号被锁定
		}

		if (result != LOGIN_RET_FAILED_NEW_CARD || result != LOGIN_RET_FAILED_USER_ONLINE)
		{
			RTW_WIDGET("fmlogin.fmpassword.txtpassword")->SetText("");
		}

		LoginErrMsg(err, szRetStr, sRetCode);

		m_bSelectCharDone = true;
	}

	unguard;
}

void GcLogin::OnNetConnect(bool bSucceeded)
{
	guard;
	if (!bSucceeded)
	{
		LoginErrMsg(GLEM_CONNECT_FAIL);
	}
	else
	{
		GcLoginSession::Login(m_szAccountUsername.c_str(), m_szAccountPassword.c_str(), 0, m_ishallLogin, m_bForceLogin);
	}
	unguard;
}

//设置强制登录
void GcLogin::SetForceLogin(bool bForceLogin)
{
	m_bForceLogin = bForceLogin;
}

void GcLogin::OnNetDisconnect()
{
	guard;
	if (m_nDisconnectCause == 1)
	{
		ShowMessage(R(WMSG_DISCONNECT_MULTI_LOGIN));
	}
	else
	{
		if (!m_bSelectCharDone)
		{
			LoginErrMsg(GLEM_DISCONNECT);
		}
	}
	unguard;
}

void GcLogin::OnNetKick(int cCause)
{
	switch (cCause)
	{
	case 1:
		m_nDisconnectCause = 1;
		break;
	default:
		break;
	}
	SetLoginState(GLS_LOADING);
}

void GcLogin::OnConnect(bool bSucceeded)
{
	guard;
	this->OnNetConnect(bSucceeded);
	unguard;
}

void GcLogin::OnDisconnect()
{
	guard;
	OnNetDisconnect();
	unguard;
}

void GcLogin::OnNetGetAccountInfo()
{
	guard;
	UpdateSelectChar();
	unguard;
}

void GcLogin::OnNetSelectCharDone()
{
	m_bSelectCharDone = true;
}

//人物角色性别---男女
void GcLogin::SetCharSex(bool bSex)
{
	if (bSex)
	{
		m_bSex = SEX_FEMALE;
	}
	else
	{
		m_bSex = SEX_MALE;
	}
}

//改变人物角色生肖
void GcLogin::ChangeCharAnimal(bool bNext)
{
	if (m_iCurSelectChar < 0)
		return;

	if (iAnimalIndex > 12 || iAnimalIndex < 0)
		iAnimalIndex = 0;

	if (bNext)
	{
		iAnimalIndex++;
		if (iAnimalIndex >= 12)
			iAnimalIndex = 0;
	}
	else
	{
		iAnimalIndex--;
		if (iAnimalIndex < 0)
			iAnimalIndex = 11;
	}

	RtwImage* pImage = g_workspace.getImageFactory()->createImage(s_pszCharAnimalName[iAnimalIndex]);
	pImage->SetBlend(true);
	RTW_WIDGET("fmcreatid1.fmanimal.lbanimal")->SetBackgroundImage(pImage);
}
//改变人物角色发型
void GcLogin::ChangeCharHair(bool bNext)
{
	if (m_iCurSelectChar < 0)
		return;

	int iCharIndex = GetCharIndexByActorID(m_listSelectChar[m_iCurSelectChar]);

	vector<SHeadModel> heads;
	if (!g_TableHeadModel.GetHeadModel(s_userID[iCharIndex], heads))
		return;

	if (m_HeadModelIndex[iCharIndex] > (int)heads.size() || m_HeadModelIndex[iCharIndex] < 0)
		m_HeadModelIndex[iCharIndex] = 0;

	int indexPre = m_HeadModelIndex[iCharIndex];

	if (bNext)
	{
		m_HeadModelIndex[iCharIndex]++;
		if (m_HeadModelIndex[iCharIndex] >= heads.size())
			m_HeadModelIndex[iCharIndex] = 0;
	}
	else
	{
		m_HeadModelIndex[iCharIndex]--;
		if (m_HeadModelIndex[iCharIndex] < 0)
			m_HeadModelIndex[iCharIndex] = heads.size() - 1;
	}

	m_listSelActor[m_iCurSelectChar]->UnloadSkin(heads[indexPre].skin.c_str());
	m_listSelActor[m_iCurSelectChar]->LoadSkin(heads[m_HeadModelIndex[iCharIndex]].skin.c_str(), true);

	RTW_WIDGET("fmcreatid2.fmhair.lbhairname")->SetText(heads[m_HeadModelIndex[iCharIndex]].name);
	//RTW_WIDGET("fmcreatid2.fmface.lbfacename")->SetText(heads[m_HeadModelIndex[iCharIndex]].name);
}

//改变人物2D头像
void GcLogin::ChangeCharImage(bool bNext)
{
	if (m_iCurSelectChar < 0)
		return;

	int iCharIndex = GetCharIndexByActorID(m_listSelectChar[m_iCurSelectChar]);

	vector<SCharImage> images;
	if (!g_TableCharImage.GetCharImage(s_userID[iCharIndex], images))
		return;

	if (m_HeadImageIndex[iCharIndex] > (int)images.size() || m_HeadImageIndex[iCharIndex] < 0)
		m_HeadImageIndex[iCharIndex] = 0;

	int indexPre = m_HeadImageIndex[iCharIndex];

	if (bNext)
	{
		m_HeadImageIndex[iCharIndex]++;
		if (m_HeadImageIndex[iCharIndex] >= images.size())
			m_HeadImageIndex[iCharIndex] = 0;
	}
	else
	{
		m_HeadImageIndex[iCharIndex]--;
		if (m_HeadImageIndex[iCharIndex] < 0)
			m_HeadImageIndex[iCharIndex] = images.size() - 1;
	}

	headImageID = m_HeadImageIndex[iCharIndex];
	std::string strImage = "ui_texture/";
	strImage += images[headImageID].imageCreate;
	strImage += ".dds";

	RtwImage* pImage = g_workspace.getImageFactory()->createImage(strImage);
	pImage->SetBlend(true);
	RTW_WIDGET("fmcreatid2.fmhead.lbpichead")->SetBackgroundImage(pImage);
}
/*----------------------------------------------------------------------------
-   Guide Net
----------------------------------------------------------------------------*/
class GcsGuideSession : public CG_TCPSession
{
public:
	GcsGuideSession(const char* szIP, int iPort)
	{
		m_szIP = szIP;
		m_iPort = iPort;
		m_fReConnectTime = 0.f;
		this->EnableCompress(false);
	}
	virtual ~GcsGuideSession()
	{
	}
	void ReConnect()
	{
		guard;
		if (m_szIP.IsEmpty())
		{
			ShowMessage(R(LMSG_CANNOT_FIND_GS_IP));
		}
		Connect(m_szIP, m_iPort);
		m_fReConnectTime = 0.f;
		unguard;
	}
	void OnProcess(float fSecond)
	{
		guard;
		if (m_fReConnectTime > 0.f)
		{
			if (GetState() != NET_STATE_DEAD && GetState() != NET_STATE_ZERO)
			{
				Close();
			}
			m_fReConnectTime -= fSecond;
			if (m_fReConnectTime <= 0.f)
			{
				ReConnect();
			}
		}
		else
		{
			Process();
		}
		unguard;
	}
	virtual	void OnConnect(bool ret)
	{
		guard;
		if (ret)
		{
			CG_CmdPacket sendCmd;
			sendCmd.BeginWrite();
			sendCmd.WriteShort(c2gs_query_gws_list);
			SendPacket(&sendCmd);
		}
		else
		{
			m_fReConnectTime = 10.f;
			// TODO: 如果有需要，应该在游戏中说明连接不上，需要处理
		}
		unguard;
	}
	virtual void OnReceive(CG_CmdPacket* packet)
	{
		guard;
		short sCmdID;
		if (packet->ReadShort(&sCmdID))
		{
			//RtwWidget* pWidget = RTW_WIDGET("fmserver.fmxmm.lbserver"); gao
			if (sCmdID == gs2c_gws_list)
			{
				/*if (pWidget)
				{
					pWidget->SetText(R(LMSG_SELECT_SERVERS));
				}*/ // gao
				m_fReConnectTime = GcLogin::OnGuideGetGameWorldServerList(packet);
				if (m_fReConnectTime <= 0.f)
				{
					m_fReConnectTime = 10.f;
				}
				else if (m_fReConnectTime > 60.f)
				{
					m_fReConnectTime = 60.f;
				}
			}
			else
			{
				/*if (pWidget)
				{
					pWidget->SetText(R(LMSG_LOGIN_FAIL_SERVER_MAINTENANCE));
				}*/ // gao
				m_fReConnectTime = 10.f;
			}
		}
		unguard;
	}
	virtual void OnDisconnect()
	{
		m_fReConnectTime = 10.f;
		// TODO: 如果有需要，应该在游戏中说明连接不上，需要处理
	}
private:
	RtString    m_szIP;
	bool        m_bIsGetData;
	int         m_iPort;
	float       m_fReConnectTime;
};

CG_TCPSession* GcLogin::ms_pGuideNet = NULL;
GcLogin::SGWServerList* GcLogin::ms_pGameWorldServerList = NULL;
long                    GcLogin::ms_lGameWorldServerCount = 0;

void GcLogin::StartGetGameWorldServer()
{
	guard;
	if (ms_pGuideNet == NULL)
	{
		char* szIP;
		int iPort;
		GetGuideHostInfo(&szIP, &iPort);
		ms_pGuideNet = RT_NEW GcsGuideSession(szIP, iPort);
		((GcsGuideSession*)ms_pGuideNet)->ReConnect();
	}
	unguard;
}

void GcLogin::EndGetGameWorldServer()
{
	DEL_ONE(ms_pGuideNet);
}

void GcLogin::OnGuideNetProcess(float fSecond)
{
	guard;
	if (ms_pGuideNet)
	{
		((GcsGuideSession*)ms_pGuideNet)->OnProcess(fSecond);
	}
	unguard;
}

float GcLogin::OnGuideGetGameWorldServerList(CG_CmdPacket* pPacket)
{
	guard;
	short sTimeSecond = 10, sCnt, sPort;
	char cEvaluation;
	char* pName, * pIP;
	SGWServerList item;
	pPacket->ReadShort(&sTimeSecond); // 先取得下次更新的时间，再取得服务器列表
	pPacket->ReadShort(&sCnt);

	ms_lGameWorldServerCount = sCnt;
	DEL_ARRAY(ms_pGameWorldServerList);
	ms_pGameWorldServerList = RT_NEW SGWServerList[ms_lGameWorldServerCount];
	for (int i = 0; i < ms_lGameWorldServerCount; i++)
	{
		pPacket->ReadString(&pName);
		pPacket->ReadString(&pIP);
		pPacket->ReadShort(&sPort);
		pPacket->ReadByte(&cEvaluation);

		ms_pGameWorldServerList[i].lPort = sPort;
		ms_pGameWorldServerList[i].szName = pName;
		ms_pGameWorldServerList[i].szIP = pIP;
		ms_pGameWorldServerList[i].lEvaluation = cEvaluation;
		//ms_pGameWorldServerList[i].ping = GetPing(pIP);
	}
	if (GetLogin())
	{
		GetLogin()->OnUIUpdateGameWorldServerList();
	}

	return sTimeSecond;
	unguard;
}
int  GcLogin::GetPing(char* zIP)
{
	guard;
	SECURITY_ATTRIBUTES sa, sa2;
	HANDLE hInputRead, hInputWrite;
	HANDLE hOutputRead, hOutputWrite;

	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;
	if (!CreatePipe(&hOutputRead, &hOutputWrite, &sa, 0))
	{
		return 1;
	}

	sa2.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa2.lpSecurityDescriptor = NULL;
	sa2.bInheritHandle = TRUE;
	if (!CreatePipe(&hInputRead, &hInputWrite, &sa2, 0))
	{
		return 1;
	}

	STARTUPINFO si;
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
	if (!CreateProcess(NULL, cmd_line, NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi))
	{
		return 1;
	}

	CloseHandle(hInputRead);
	CloseHandle(hOutputWrite);

	char buffer[4096] = { 0 };
	char bufferStart[] = " time";
	char bufferEnd[] = "ms ";
	char ping[10] = { 0 };
	DWORD bytesRead;
	int i = 1;
	int pingNum = 0;
	while (true)
	{
		if (ReadFile(hOutputRead, buffer, 4095, &bytesRead, NULL) == NULL)
		{
			break;
		}
		// 字符串解译
		// 第一次查找
		char* startPos = strstr(buffer, bufferStart);
		while (startPos)
		{
			char* endPos = strstr(startPos, bufferEnd);
			if (endPos)
			{
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

void GcLogin::OnUIUpdateGameWorldServerList()
{
	guard;
	if (m_eStatus != GLS_SELECT_GAMEWORLD_SERVER) return;
	int i;
	char szEvaluation[40];

	//RtwWidget* pWidget = RTW_WIDGET("fmserver.fmxmm.lbserver");
	//if (pWidget)
	//{
	//    if (ms_lGameWorldServerCount==0)
	//    {
	//        pWidget->SetText(R(LMSG_LOGIN_FAIL_SERVER_MAINTENANCE)); // 服务器维护中
	//    }else
	//    {
	//        pWidget->SetText(R(LMSG_SELECT_SERVERS)); // 请选择服务器
	//    }
	//} gao

	/* gao 2010.1.14
	服务器刷新方式修改
	*/
	i = 0;
	RtwRect imgRc(448, 0, 501, 0);
	g_layerLogin->m_formServer->Refresh();
	while (i < ms_lGameWorldServerCount)
	{
		std::string szName = ms_pGameWorldServerList[i].szName;
		//g_layerLogin->m_pServerListButton[i]->SetText(szName); gao
		switch (ms_pGameWorldServerList[i].lEvaluation)
		{
		case -1:
			//strcpy(szEvaluation, "维护");//by fox for string
			rt2_strncpy(szEvaluation, R(MSG_SERVER_VINDICATE), 40);
			imgRc.bottom = 58;
			imgRc.top = 54;
			break;
		case 0: //    0
		case 1: //  400
		case 2: //  800
		case 3: // 1200
		case 4: // 1600
		default:
			//strcpy(szEvaluation, "普通");//by fox for string
			rt2_strncpy(szEvaluation, R(MSG_SERVER_NORMAL), 40);
			imgRc.bottom = 44;
			imgRc.top = 40;
			break;
		case 5: // 2000
		case 6: // 2400
		case 7: // 2800
		case 8: // 3200
		case 9: // 3600
			//strcpy(szEvaluation, "繁忙");//by fox for string
			rt2_strncpy(szEvaluation, R(MSG_SERVER_BUSY), 40);
			imgRc.bottom = 30;
			imgRc.top = 26;
			break;
		case 10: // 4000 人以上
			//strcpy(szEvaluation, "满员");//by fox for string
			rt2_strncpy(szEvaluation, R(MSG_SERVER_FULL), 40);
			imgRc.bottom = 16;
			imgRc.top = 12;
			break;
		}
		//g_layerLogin->m_pServerStatus[i]->SetText(szEvaluation); gao
		g_layerLogin->m_formServer->OnInsertNewServer(szName.c_str(), ms_pGameWorldServerList[i].ping, szEvaluation, imgRc);
		++i;
	}
	//从文件中读取上次选中的服务器
	RtIni iniUser;
	const char* szLastServer = 0;
	if (iniUser.OpenFile(R(INI_USER)))
	{
		szLastServer = iniUser.GetEntry("server", "select");
		if (szLastServer)
		{
			m_iLastServer = atol(szLastServer);
		}
		iniUser.CloseFile();
	}
	if (m_iLastServer >= 0)
	{
		g_layerLogin->m_formServer->SetSelectItemLast(m_iLastServer);
	}
	/* end */
	//gao
	//for (; i<UILayerLogin::MAX_BUTTON; i++)
	//{
	//    g_layerLogin->m_pServerListButton[i]->Hide();
	//    g_layerLogin->m_pServerStatus[i]->Hide();
	//}
	unguard;
}

void GcLogin::OnPoseBegin(SRT_Pose* pose)
{
	guard;
	//g_pMusicThread->Play("login_bg.ogg", true);
	if (m_eStatus == GLS_CREATE_CHAR)
	{
		RTW_WIDGET("lbbackground")->Hide();
		RTW_WIDGET("btnmale")->Hide();
		RTW_WIDGET("btnfemale")->Hide();
		RTW_WIDGET("fmcreatid1")->Hide();
		RTW_WIDGET("fmcreatid2")->Hide();
		RTW_WIDGET("fmcreatid3")->Hide();
		RTW_WIDGET("fmjob")->Hide();
		if (pose->Name == "otooh" || pose->Name == "stooh" || pose->Name == "wtooh" || pose->Name == "mtooh")
		{
			RTW_WIDGET("fmhuajian.btnhuajian")->Enable();
			RTW_WIDGET("fmshushan.btnshushan")->Disable();
			RTW_WIDGET("fmshengwu.btnshengwu")->Disable();
			RTW_WIDGET("fmmingwang.btnmingwang")->Disable();
		}
		else if (pose->Name == "otoos" || pose->Name == "htoos" || pose->Name == "wtoos" || pose->Name == "mtoos")
		{
			RTW_WIDGET("fmhuajian.btnhuajian")->Disable();
			RTW_WIDGET("fmshushan.btnshushan")->Enable();
			RTW_WIDGET("fmshengwu.btnshengwu")->Disable();
			RTW_WIDGET("fmmingwang.btnmingwang")->Disable();
		}
		else if (pose->Name == "otoom" || pose->Name == "htoom" || pose->Name == "stoom" || pose->Name == "wtoom")
		{
			RTW_WIDGET("fmhuajian.btnhuajian")->Disable();
			RTW_WIDGET("fmshushan.btnshushan")->Disable();
			RTW_WIDGET("fmshengwu.btnshengwu")->Enable();
			RTW_WIDGET("fmmingwang.btnmingwang")->Disable();
		}
		else if (pose->Name == "otoow" || pose->Name == "htoow" || pose->Name == "stoow" || pose->Name == "mtoow")
		{
			RTW_WIDGET("fmhuajian.btnhuajian")->Disable();
			RTW_WIDGET("fmshushan.btnshushan")->Disable();
			RTW_WIDGET("fmshengwu.btnshengwu")->Disable();
			RTW_WIDGET("fmmingwang.btnmingwang")->Enable();
		}
	}
	unguard;
}

void GcLogin::OnPoseEnd(SRT_Pose* pose)
{
	guard;
	if (m_pCamera)
	{
		m_pCamera = NULL;
	}
	iAnimalIndex = rand() % 12;
	iRandomAnimalIndex = iAnimalIndex;
	RtwImage* pImage = g_workspace.getImageFactory()->createImage("ui/textures/11.tga");
	RtwImage* pImage1 = g_workspace.getImageFactory()->createImage("ui/textures/11_a.tga");
	if (m_eStatus == GLS_CREATE_CHAR)
	{
		if (pose->Name == "otooh" || pose->Name == "stooh" || pose->Name == "wtooh" || pose->Name == "mtooh")
		{
			g_pMusicThread->Play("bgm_005_a.ogg", true);
			RTW_WIDGET("lbbackground")->Show();
			RTW_WIDGET("btnmale")->Show();
			RTW_WIDGET("btnfemale")->Show();
			RTW_WIDGET("fmcreatid1")->Show();
			RTW_WIDGET("fmcreatid2")->Show();
			RTW_WIDGET("fmcreatid3")->Show();
			RTW_WIDGET("fmjob")->Show();
			if (pImage)
			{
				//SetSourceRect的参数是左上角和右下角两个点
				pImage->SetSourceRect(RtwRect(158, 249, 471, 399));
				pImage->SetBlend(true);
				RTW_WIDGET("fmjob.lbtxtjob")->SetBackgroundImage(pImage);
			}
			RTW_WIDGET("btnmale")->Disable();
			RTW_WIDGET("btnfemale")->Enable();
			RTW_WIDGET_T(CUiCheckButton, "btnfemale")->SetChecked(true);
			SetCharSex(true);

			RTW_WIDGET("fmhuajian.btnhuajian")->Enable();
			RTW_WIDGET("fmshushan.btnshushan")->Enable();
			RTW_WIDGET("fmshengwu.btnshengwu")->Enable();
			RTW_WIDGET("fmmingwang.btnmingwang")->Enable();
			RTW_WIDGET_T(CUiCheckButton, "fmshushan.btnshushan")->SetChecked(false);
			RTW_WIDGET_T(CUiCheckButton, "fmhuajian.btnhuajian")->SetChecked(true);
			RTW_WIDGET_T(CUiCheckButton, "fmshengwu.btnshengwu")->SetChecked(false);
			RTW_WIDGET_T(CUiCheckButton, "fmmingwang.btnmingwang")->SetChecked(false);
			//默认显示人物2D图像
			pImage = g_workspace.getImageFactory()->createImage("ui_texture/pic_role_hj_f_01_f.dds");
			pImage->SetBlend(true);
			RTW_WIDGET("fmcreatid2.fmhead.lbpichead")->SetBackgroundImage(pImage);
			pImage = g_workspace.getImageFactory()->createImage(s_pszCharAnimalName[iAnimalIndex]);
			pImage->SetBlend(true);
			RTW_WIDGET("fmcreatid1.fmanimal.lbanimal")->SetBackgroundImage(pImage);
			//播放人物动画
			if (m_pWeaponHL)
			{
				m_pWeaponHL->PlayPose("login_s0", false);
			}

			CRT_ActorInstance* hjActor = GetSelectedActorByActorID(HUA_JIAN_ID);
			if (hjActor)
			{
				hjActor->PlayPose("login_s0", false);
			}
		}
		else if (pose->Name == "otoos" || pose->Name == "htoos" || pose->Name == "wtoos" || pose->Name == "mtoos")
		{
			g_pMusicThread->Play("bgm_004_a.ogg", true);
			RTW_WIDGET("lbbackground")->Show();
			RTW_WIDGET("btnmale")->Show();
			RTW_WIDGET("btnfemale")->Show();
			RTW_WIDGET("fmcreatid1")->Show();
			RTW_WIDGET("fmcreatid2")->Show();
			RTW_WIDGET("fmcreatid3")->Show();
			RTW_WIDGET("fmjob")->Show();
			if (pImage)
			{
				//SetSourceRect的参数是左上角和右下角两个点
				pImage->SetSourceRect(RtwRect(160, 5, 505, 240));
				pImage->SetBlend(true);
				RTW_WIDGET("fmjob.lbtxtjob")->SetBackgroundImage(pImage);
			}
			RTW_WIDGET("btnmale")->Enable();
			RTW_WIDGET_T(CUiCheckButton, "btnmale")->SetChecked(true);
			SetCharSex(false);
			RTW_WIDGET("btnfemale")->Disable();

			RTW_WIDGET("fmhuajian.btnhuajian")->Enable();
			RTW_WIDGET("fmshushan.btnshushan")->Enable();
			RTW_WIDGET("fmshengwu.btnshengwu")->Enable();
			RTW_WIDGET("fmmingwang.btnmingwang")->Enable();
			RTW_WIDGET_T(CUiCheckButton, "fmhuajian.btnhuajian")->SetChecked(false);
			RTW_WIDGET_T(CUiCheckButton, "fmshushan.btnshushan")->SetChecked(true);
			RTW_WIDGET_T(CUiCheckButton, "fmshengwu.btnshengwu")->SetChecked(false);
			RTW_WIDGET_T(CUiCheckButton, "fmmingwang.btnmingwang")->SetChecked(false);
			//默认显示人物2D图像
			pImage = g_workspace.getImageFactory()->createImage("ui_texture/pic_role_ss_m_01_f.dds");
			pImage->SetBlend(true);
			RTW_WIDGET("fmcreatid2.fmhead.lbpichead")->SetBackgroundImage(pImage);
			pImage = g_workspace.getImageFactory()->createImage(s_pszCharAnimalName[iAnimalIndex]);
			pImage->SetBlend(true);
			RTW_WIDGET("fmcreatid1.fmanimal.lbanimal")->SetBackgroundImage(pImage);
			//播放人物动画
			m_pWeaponFLWay->PlayPose("login_l0", false);

			CRT_ActorInstance* shActor = GetSelectedActorByActorID(SHU_SHAN_ID);
			if (shActor)
				shActor->PlayPose("login_l0", false);
		}
		else if (pose->Name == "otoom" || pose->Name == "htoom" || pose->Name == "stoom" || pose->Name == "wtoom")
		{
			g_pMusicThread->Play("bgm_015_a.ogg", true);
			RTW_WIDGET("lbbackground")->Show();
			RTW_WIDGET("btnmale")->Show();
			RTW_WIDGET("btnfemale")->Show();
			RTW_WIDGET("fmcreatid1")->Show();
			RTW_WIDGET("fmcreatid2")->Show();
			RTW_WIDGET("fmcreatid3")->Show();
			RTW_WIDGET("fmjob")->Show();
			if (pImage1)
			{
				//SetSourceRect的参数是左上角和右下角两个点
				pImage1->SetSourceRect(RtwRect(161, 250, 510, 480));
				pImage1->SetBlend(true);
				RTW_WIDGET("fmjob.lbtxtjob")->SetBackgroundImage(pImage1);
			}
			RTW_WIDGET("btnmale")->Disable();
			RTW_WIDGET("btnfemale")->Enable();
			RTW_WIDGET_T(CUiCheckButton, "btnfemale")->SetChecked(true);
			SetCharSex(true);

			RTW_WIDGET("fmhuajian.btnhuajian")->Enable();
			RTW_WIDGET("fmshushan.btnshushan")->Enable();
			RTW_WIDGET("fmshengwu.btnshengwu")->Enable();
			RTW_WIDGET("fmmingwang.btnmingwang")->Enable();
			RTW_WIDGET_T(CUiCheckButton, "fmshushan.btnshushan")->SetChecked(false);
			RTW_WIDGET_T(CUiCheckButton, "fmhuajian.btnhuajian")->SetChecked(false);
			RTW_WIDGET_T(CUiCheckButton, "fmshengwu.btnshengwu")->SetChecked(true);
			RTW_WIDGET_T(CUiCheckButton, "fmmingwang.btnmingwang")->SetChecked(false);
			//默认显示人物2D图像
			pImage = g_workspace.getImageFactory()->createImage("ui_texture/pic_role_mj_f_01_f.dds");
			pImage->SetBlend(true);
			RTW_WIDGET("fmcreatid2.fmhead.lbpichead")->SetBackgroundImage(pImage);
			pImage = g_workspace.getImageFactory()->createImage(s_pszCharAnimalName[iAnimalIndex]);
			pImage->SetBlend(true);
			RTW_WIDGET("fmcreatid1.fmanimal.lbanimal")->SetBackgroundImage(pImage);
			//播放人物动画
			if (m_pWeaponMJ)
			{
				m_pWeaponMJ->PlayPose("login_z0", false);
			}
			CRT_ActorInstance* mjActor = GetSelectedActorByActorID(MIAO_JIANG_ID);
			if (mjActor)
				mjActor->PlayPose("login_z0", false);
		}
		else if (pose->Name == "otoow" || pose->Name == "htoow" || pose->Name == "stoow" || pose->Name == "mtoow")
		{
			g_pMusicThread->Play("bgm_014_a.ogg", true);
			RTW_WIDGET("lbbackground")->Show();
			RTW_WIDGET("btnmale")->Show();
			RTW_WIDGET("btnfemale")->Show();
			RTW_WIDGET("fmcreatid1")->Show();
			RTW_WIDGET("fmcreatid2")->Show();
			RTW_WIDGET("fmcreatid3")->Show();
			RTW_WIDGET("fmjob")->Show();
			if (pImage1)
			{
				//SetSourceRect的参数是左上角和右下角两个点
				pImage1->SetSourceRect(RtwRect(168, 6, 511, 236));
				pImage1->SetBlend(true);
				RTW_WIDGET("fmjob.lbtxtjob")->SetBackgroundImage(pImage1);
			}
			RTW_WIDGET("btnmale")->Enable();
			RTW_WIDGET_T(CUiCheckButton, "btnmale")->SetChecked(true);
			SetCharSex(false);
			RTW_WIDGET("btnfemale")->Disable();

			RTW_WIDGET("fmhuajian.btnhuajian")->Enable();
			RTW_WIDGET("fmshushan.btnshushan")->Enable();
			RTW_WIDGET("fmshengwu.btnshengwu")->Enable();
			RTW_WIDGET("fmmingwang.btnmingwang")->Enable();
			RTW_WIDGET_T(CUiCheckButton, "fmhuajian.btnhuajian")->SetChecked(false);
			RTW_WIDGET_T(CUiCheckButton, "fmshushan.btnshushan")->SetChecked(false);
			RTW_WIDGET_T(CUiCheckButton, "fmshengwu.btnshengwu")->SetChecked(false);
			RTW_WIDGET_T(CUiCheckButton, "fmmingwang.btnmingwang")->SetChecked(true);
			//默认显示人物2D图像
			pImage = g_workspace.getImageFactory()->createImage("ui_texture/pic_role_wt_m_01_f.dds");
			pImage->SetBlend(true);
			RTW_WIDGET("fmcreatid2.fmhead.lbpichead")->SetBackgroundImage(pImage);
			pImage = g_workspace.getImageFactory()->createImage(s_pszCharAnimalName[iAnimalIndex]);
			pImage->SetBlend(true);
			RTW_WIDGET("fmcreatid1.fmanimal.lbanimal")->SetBackgroundImage(pImage);
			//播放人物动画
			if (m_pWeaponWT)
			{
				m_pWeaponWT->PlayPose("login_f0", false);
			}
			CRT_ActorInstance* wtActor = GetSelectedActorByActorID(WU_TAI_ID);
			if (wtActor)
				wtActor->PlayPose("login_f0", false);
		}
	}
	unguard;
}

void GcLogin::OnPoseEvent(SRT_Pose* pose, SRT_PoseEvent* event)
{
}

void GcLogin::OnPoseBreak(SRT_Pose* oldPose, SRT_Pose* newPose)
{
}

void GcLogin::SetSelectUser(int iSel)
{
	if (iSel == m_iCurSelectChar)
	{
		return;
	}
	const char* poseName = NULL;
	switch (m_eStatus)
	{
	case GLS_SELECT_CHAR:
	{
		poseName = GetCameraPoseName(iSel, true);
		if (poseName && (m_pCamera = FindModel("Camera")))
		{
			m_pCamera->RegisterNotify(this);
			if (!m_pCamera->PlayPose(poseName, false, 1.f))
				m_pCamera->RegisterNotify(NULL);
		}
		m_iCurSelectChar = iSel;
		UpdateSelectChar();
	}
	break;
	case GLS_CREATE_CHAR:
	{
		poseName = GetCameraPoseName(iSel, false);
		if (poseName && (m_pCamera = FindModel("Camera")))
		{
			m_pCamera->RegisterNotify(this);
			if (!m_pCamera->PlayPose(poseName, false, 1.f))
			{
				m_pCamera->RegisterNotify(NULL);
			}
		}
		m_iCurSelectChar = iSel;

		//默认显示人物发型和脸型
		// Modified by Wayne Wong 2010-11-26
		//int iCharIndex = GetCharIndexByActorID(m_listSelectChar[m_iCurSelectChar]);
		//vector<SHeadModel> heads;
		//if (!g_TableHeadModel.GetHeadModel(s_userID[iCharIndex], heads))
		//	return;

		vector<SHeadModel> heads;
		if (!g_TableHeadModel.GetHeadModel(s_userID[m_iCurSelectChar], heads))
			return;

		//m_listSelActor[m_iCurSelectChar]->UnloadSkin(heads[m_ePrevHeadID].skin.c_str());
		//m_listSelActor[m_iCurSelectChar]->LoadSkin(heads[0].skin.c_str(), true);
		RTW_WIDGET("fmcreatid2.fmhair.lbhairname")->SetText(heads[0].name);
		RTW_WIDGET("fmcreatid2.fmface.lbfacename")->SetText("暂不开放");
		m_ePrevHeadID = m_eNextHeadID = 0;
	}
	break;
	default:
		break;
	}
}

int GcLogin::GetCharIndexByActorID(short actorID)
{
	int iCharIndex = 0;
	for (int i = 0; i < sizeof(s_userID) / sizeof(s_userID[0]); ++i)
	{
		if (s_userID[i] == actorID)
			return i;
	}
	return iCharIndex;
}

const char* GcLogin::GetCameraPoseName(int iSel, bool bSelectChar)
{
	switch (iSel)
	{
	case 0: //五台男
		switch (m_iCurSelectChar)
		{
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
	case 1://花间女
		switch (m_iCurSelectChar)
		{
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
	case 2://蜀山男
		switch (m_iCurSelectChar)
		{
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
	case 3://苗疆女
		switch (m_iCurSelectChar)
		{
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

CRT_ActorInstance* GcLogin::GetSelectedActorByActorID(short actorID)
{
	for (size_t index = 0; index < m_listSelectChar.size(); ++index)
	{
		if (m_listSelectChar[index] == actorID)
			return m_listSelActor[index];
	}

	return NULL;
}
/*----------------------------------------------------------------------------
-   End.
----------------------------------------------------------------------------*/


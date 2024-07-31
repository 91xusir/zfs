#include "gc_include.h"
#include "gc_login.h"
#include "region_client.h"
#include "gc_login_session.h"
#include "ui_form_msg.h"
#include "ui_layer_select_char.h"

using namespace gc_net;

#define BEGIN_CMD_FUNC(name)                                                                       \
    bool GcLoginSession::name(CG_CmdPacket* packet) {                                              \
        guard;
#define END_CMD_FUNC                                                                               \
    ;                                                                                              \
    return true;                                                                                   \
    unguard;                                                                                       \
    }

BEGIN_CMD_FUNC(cmd_g2c_login_ret) {
    long result;

    if (!packet->ReadLong(&result))
        return false;

    char* szRetStr = NULL;
    short sRetCode = 0;
    if (result == -11)  // LOGIN_RET_FAILED_USERDEFINE
    {
        if (!packet->ReadString(&szRetStr)) {
            szRetStr = NULL;
        }
    } else {
        packet->ReadShort(&sRetCode);
    }

    char cPointType = -1;
    long lPoint = 0;
    if (result == 0)  // LOGIN_RET_SUCCESS
    {
        packet->ReadByte(&cPointType);
        packet->ReadLong(&lPoint);
    }

    GetLogin()->OnNetLogin(result, szRetStr, sRetCode, cPointType, lPoint);
}

END_CMD_FUNC;

BEGIN_CMD_FUNC(cmd_g2c_char_password_confirm_ret) {
    long result = 0;

    if (!packet->ReadLong(&result))
        return false;

    g_layerSelectChar->OnReceivePasswordConfirm(result);
}

END_CMD_FUNC;

BEGIN_CMD_FUNC(cmd_g2c_char_password_set_ret) {
    char result = 0;

    if (!packet->ReadByte(&result))
        return false;

    g_layerSelectChar->OnReceiveSetPassword(result);
}

END_CMD_FUNC;

BEGIN_CMD_FUNC(cmd_g2c_char_password_del_ret) {
    char result = 0;

    if (!packet->ReadByte(&result))
        return false;

    g_layerSelectChar->OnReceiveDelPassword(result);
}

END_CMD_FUNC;

BEGIN_CMD_FUNC(cmd_g2c_char_password_change_ret) {
    char result = 0;

    if (!packet->ReadByte(&result))
        return false;

    if (result == 1) {
        UIFormMsg::ShowStatic(R(LMSG_CHANGE_CHAR_PASSWORD_OK), UIFormMsg::TYPE_OK);
    } else {
        UIFormMsg::ShowStatic(R(LMSG_CHANGE_CHAR_PASSWORD_FAILED), UIFormMsg::TYPE_OK);
    }
}

END_CMD_FUNC;

BEGIN_CMD_FUNC(cmd_g2c_create_char_ret) {
    long  result;
    long  charID;
    char* items;
    char* name;
    char* attributes;
    short headModelID;
    short headImageID;

    if (!packet->ReadLong(&result))
        return false;

    // result = 0 成功
    // result = 1 角色名称已经存在
    // result = 2 其他的错误

    if (result != gws_create_user_okay) {
        GcLogin::EErrMsg errMsg;

        switch (result) {
            default:
                errMsg = GcLogin::GLEM_CREATE_RESULT_FAIL;
                break;
            case gws_create_user_error:
                errMsg = GcLogin::GLEM_CREATE_RESULT_FAIL;
                break;
            case gws_create_user_invalid_name:
                errMsg = GcLogin::GLEM_CREATE_RESULT_INVALID_NAME;
                break;
            case gws_create_user_duplicate_name:
                errMsg = GcLogin::GLEM_CREATE_RESULT_EXIST;
                break;
            case gws_create_user_faction_error:
                errMsg = GcLogin::GLEM_CREATE_FACTION_ERROR;
                break;
            case gws_create_user_banned_name:
                errMsg = GcLogin::GLEM_CREATE_BANNED_NAME;
                break;
        }

        GetLogin()->LoginErrMsg(errMsg);
        return true;
    }
    GetLogin()->QueryAccountInfo();

    if (!packet->ReadLong(&charID))
        return false;

    for (int n = 0; n < m_account.chatCount; n++) {
        if (m_account.users[n].id == charID) {
            return true;
        }
    }

    if (!packet->ReadString(&name))
        return false;

    int length;
    if (!packet->ReadBinary(&attributes, &length))
        return false;

    if (!packet->ReadString(&items))
        return false;

    if (!packet->ReadShort(&headModelID))
        return false;

    if (!packet->ReadShort(&headImageID))
        return false;

    m_account.users[m_account.chatCount].id = charID;
    m_account.users[m_account.chatCount].headModelID = headModelID;
    m_account.users[m_account.chatCount].name = name;
    m_account.users[m_account.chatCount].items = items;
    m_account.users[m_account.chatCount].attributes = *(SUserAttr*)attributes;
    m_account.users[m_account.chatCount].deleteDate = "";
    m_account.chatCount++;

    // 角色信息已经加入本地帐号信息
    GetLogin()->SetLastSelectCharID(((SUserAttr*)attributes)->metier);
    //进入选人界面
    GetLogin()->SetLoginState(GcLogin::GLS_SELECT_CHAR);
}

END_CMD_FUNC;

BEGIN_CMD_FUNC(cmd_g2c_restore_char_ret) {
    char result;

    if (!packet->ReadByte(&result))
        return false;

    if (result == 1) {
        GetLogin()->OnRestoreCharSuccess();
    } else {
        GetLogin()->LoginErrMsg(GcLogin::GLEM_RESTORE_RESULT_FAIL);
    }
}

END_CMD_FUNC;

BEGIN_CMD_FUNC(cmd_g2c_delete_char_ret) {
    long result;
    long charID;
    char hasDelete = 1;

    if (!packet->ReadLong(&result))
        return false;

    if (!packet->ReadLong(&charID))
        return false;

    if (!packet->ReadByte(&hasDelete))
        return false;
    // result == 0 成功
    // result == 1 失败

    if (result == 1) {
        LOG1("Login: 从服务器中删除角色失败 (CharID=%d)\n", charID);
        // TODO: 删除角色失败处理
        GetLogin()->LoginErrMsg(GcLogin::GLEM_DELETE_RESULT_FAIL);
    }

    // 从现在帐号数据删除角色信息
    int n;
    for (n = 0; n < m_account.chatCount; n++) {
        if (m_account.users[n].id == charID) {
            break;
        }
    }

    // 未找到所要删除的角色信息，可能角色被多次删除
    if (n == m_account.chatCount) {
        LOG1("Login: 角色已经从本地帐号信息中删除 (CharID=%d)", charID);
        return true;
    } else {
        if (hasDelete) {
            for (n++; n < m_account.chatCount; n++) {
                m_account.users[n - 1] = m_account.users[n];
            }
            m_account.chatCount--;

            GetLogin()->SetSelectUser(-1);
        } else {
            int iSel = GetLogin()->GetCurSelectChar();
            GetLogin()->SetCurSelectChar(-1);
        }
        GetLogin()->OnNetDeleteUser(charID, hasDelete);
    }
}

END_CMD_FUNC;

BEGIN_CMD_FUNC(cmd_g2c_query_account_ret) {
    long chatCount;

    if (!packet->ReadLong(&chatCount))
        return false;

    if (chatCount > 4)
        return false;

    int n;
    m_account.chatCount = chatCount;

    for (n = 0; n < chatCount; n++) {
        char* name;
        char* items;
        char* delDate;
        short headModelID;
        char  hasCharPwd;  //人物角色密码锁
        char  isFrozen;    //人物角色是否冷冻状态

        if (!packet->ReadLong(&m_account.users[n].id))
            return false;

        m_account.users[n].headModelID = 0;

        if (!packet->ReadString(&name))
            return false;

        int   length;
        char* attributes;
        if (!packet->ReadBinary(&attributes, &length))
            return false;

        if (!packet->ReadString(&items))
            return false;

        if (!packet->ReadString(&delDate))
            return false;

        if (!packet->ReadShort(&headModelID))
            return false;

        if (!packet->ReadByte(&hasCharPwd))
            return false;

        if (!packet->ReadByte(&isFrozen))
            return false;

        m_account.users[n].name = name;
        m_account.users[n].items = items;
        m_account.users[n].attributes = *(SUserAttr*)attributes;
        m_account.users[n].deleteDate = delDate;
        //检测模型和头像匹配
        SHeadModel headModel;
        if (!g_TableHeadModel.GetHeadModelByID(headModelID, headModel))
            return false;
        if (headModel.actorModelID == m_account.users[n].attributes.actorID) {
            m_account.users[n].headModelID = headModelID;
        } else {
            vector<SHeadModel> heads;
            if (!g_TableHeadModel.GetHeadModel(m_account.users[n].attributes.actorID, heads))
                return false;
            m_account.users[n].headModelID = heads[0].id;
        }

        m_account.users[n].hasCharPwd = hasCharPwd;
        m_account.users[n].isFrozen = isFrozen;
    }

    if (m_account.chatCount > 0) {
        GetLogin()->SetLoginState(GcLogin::GLS_SELECT_CHAR);
    } else {
        GetLogin()->SetLoginState(GcLogin::GLS_CREATE_CHAR);
    }
}

END_CMD_FUNC;

BEGIN_CMD_FUNC(cmd_g2c_select_char_ret) {
    long charID;

    if (!packet->ReadLong(&charID))
        return false;

    if (charID) {
        long  seed;
        char* regionHost;
        short regionPort;

        if (!packet->ReadLong(&seed))
            return false;

        if (!packet->ReadString(&regionHost))
            return false;

        if (!packet->ReadShort(&regionPort))
            return false;

        // 进入此处,LoginSession任务完成了，会被GWServer断开连接，连RegionServer了
        GetLogin()->SetLoginState(GcLogin::GLS_NONE);
        GetLogin()->OnNetSelectCharDone();
        ChangeGameFlow(true);
        SetRegionServerInfo(charID, seed, regionHost, regionPort);
    } else {
        char* szErrMsg;

        if (!packet->ReadString(&szErrMsg))
            return false;

        UILayer::LeaveLoading();
        ShowMessage(szErrMsg);
    }
}

END_CMD_FUNC;

BEGIN_CMD_FUNC(cmd_g2c_decline) {
    // 目前这段代码没有被执行，因为服务器发送完这个命令后就立即杀掉连接，导致数据还没有传送出来就被清除了
    //     目前也不修改这段代码，因为没有必要
    char cCause;
    if (!packet->ReadByte(&cCause)) {
        cCause = 0;
    }
    switch (cCause) {
        default:
        case 0:  // 服务器维护
            ShowMessage(R(LMSG_LOGIN_FAIL_SERVER_MAINTENANCE));
            break;
        case 1:  // ip拒绝
            ShowMessage(R(LMSG_LOGIN_FAIL_IP_DECLINE));
            break;
        case 2:  // 服务器连接容量满
            ShowMessage(R(LMSG_LOGIN_FAIL_SERVER_FULL));
            break;
    }
    return true;
}

END_CMD_FUNC;

BEGIN_CMD_FUNC(cmd_g2c_session_close) {
    // 目前这段代码没有被执行，因为服务器发送完这个命令后就立即杀掉连接，导致数据还没有传送出来就被清除了
    //     目前也不修改这段代码，因为没有必要
    char cCause;
    if (!packet->ReadByte(&cCause)) {
        cCause = 0;
    }
    switch (cCause) {
        default:
        case 0:  // 服务器维护
            ShowMessage(R(LMSG_LOGIN_FAIL_SERVER_MAINTENANCE));
            break;
        case 1:  // ip拒绝
            ShowMessage(R(LMSG_LOGIN_FAIL_IP_DECLINE));
            break;
        case 2:  // 服务器连接容量满
            ShowMessage(R(LMSG_LOGIN_FAIL_SERVER_FULL));
            break;
    }
    return true;
}

END_CMD_FUNC;

BEGIN_CMD_FUNC(cmd_g2c_kick) {
    //强制登录,踢人在登录时.
    char cCause = 0;

    if (!packet->ReadByte(&cCause))
        return false;

    GetLogin()->OnNetKick(cCause);
    return true;
}

END_CMD_FUNC;

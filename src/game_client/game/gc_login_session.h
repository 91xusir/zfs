#ifndef _INC_GC_LOGIN_SESSION_H_
#define _INC_GC_LOGIN_SESSION_H_

#include "gw_protocol.h"

namespace gc_net {

struct GcAccountInfo {
    RtString account;
    long     chatCount;

    struct ChatInfo {
        long      id;
        short     headModelID;
        RtString  name;
        SUserAttr attributes;
        RtString  items;
        RtString  deleteDate;
        char      hasCharPwd;  //人物角色密码锁
        char      isFrozen;    //人物角色是否冷冻状态
    } users[8];
};

class GcLoginSession : protected CG_TCPSession {
    typedef bool (GcLoginSession::*CommandCallback)(CG_CmdPacket*);

    struct Command {
        short           id;
        CommandCallback callback;
    };

   public:
    bool ConnectServer(const char* host, int port);

   protected:
    virtual void OnConnect(bool ret);

    virtual void OnReceive(CG_CmdPacket* packet);

    virtual void OnDisconnect();

   public:
    ///
    bool Login(const char* username, const char* password, char code, bool ishall = false,
               bool bForceLogin = false);

    ///
    bool QueryAccountInfo();

    //人物角色创建
    bool CreateChar(const char* name, char metier, char bIsS, char bSex, short headModelID,
                    const char* password, short iAnimal, short iImage);

    ///
    bool DeleteChar(long id);

    ///
    bool RestoreChar(long id);

    ///
    bool SelectChar(long id);

    ///
    bool SetCharPassword(long id, const char* password);

    bool ChangeCharPassword(long id, const char* old, const char* newpassword);

    bool DelCharPassword(long id, const char* password);

    ///
    bool CharPasswordConfirm(long id, const char* password);

    GcAccountInfo& GetAccountInfo() { return m_account; }

   public:
    ///
    CG_CmdPacket& BeginSend() {
        m_sendCmd.BeginWrite();
        return m_sendCmd;
    }

    ///
    bool EndSend() { return SendPacket(&m_sendCmd); }

    ///
    CG_CmdPacket& GetSendCmd() { return m_sendCmd; }

    ///
    CG_CmdPacket& GetRecvCmd() { return m_recvCmd; }

    //std::string GetMD5StringString(std::string sstring);
    std::string EncryptString(std::string sstring);

   private:
    void RegisterCommand(short id, CommandCallback callback);

    void InitChannel();

   public:
    GcLoginSession();

   private:
    ///
    CG_CmdPacket m_sendCmd;

    ///
    CG_CmdPacket m_recvCmd;

    ///
    list<Command> m_commands;

    GcAccountInfo m_account;

   private:
    time_t m_lastConnectTime;
#pragma push_macro("CMD")

#define CMD(cmd) bool cmd(CG_CmdPacket*)

    CMD(cmd_g2c_login_ret);
    CMD(cmd_g2c_restore_char_ret);
    CMD(cmd_g2c_delete_char_ret);
    CMD(cmd_g2c_create_char_ret);
    CMD(cmd_g2c_query_account_ret);
    CMD(cmd_g2c_select_char_ret);
    CMD(cmd_g2c_decline);
    CMD(cmd_g2c_session_close);
    CMD(cmd_g2c_char_password_change_ret);
    CMD(cmd_g2c_char_password_confirm_ret);
    CMD(cmd_g2c_char_password_set_ret);
    CMD(cmd_g2c_char_password_del_ret);
    CMD(cmd_g2c_kick);

#pragma pop_macro("CMD")
};

}  // namespace gc_net

using namespace gc_net;

#endif  // _INC_GC_LOGIN_SESSION_H_

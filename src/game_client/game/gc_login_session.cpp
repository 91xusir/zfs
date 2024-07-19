#include "../game/Encrypt.h"
#include "gc_include.h"
#include "region_client.h"
#include "gc_login_session.h"
#include "gc_constant.h"
using namespace gc_net;

GcLoginSession::GcLoginSession()
{
    m_lastConnectTime = 0;
	m_account.chatCount = 0;

	InitChannel();
}

bool GcLoginSession::ConnectServer(const char* host, int port)
{
    this->EnableCompress(false);

    time_t now = time(NULL);
    if (now-m_lastConnectTime > 5) // 每5秒才能登陆一次
    {
        m_lastConnectTime = now;
        Close();
        LOG2("GcLogin::Login to [%s:%d]\n", host, port);
        return CG_TCPSession::Connect((char*)host, port);
    }
    return true;
}

//tim.yang  pwlogin
//std::string GcLoginSession::GetMD5StringString(std::string sstring)
//{
//	if (sstring.empty())
//	{
//		return "";
//	}
//	char* szString = (char*)sstring.c_str();
//	char szMD5[40];
//	RtMD5 rtMD5;
//	rtMD5.EncodeData(szString, (int)strlen(szString));
//	char buff[33];
//	memset(buff, 0, 33);
//	rtMD5.GetMD5String(buff);
//	memcpy(szMD5, buff, 33);
//	RtString temp = szMD5;
//	temp.MakeLower();
//	return temp;
//}

std::string GcLoginSession::EncryptString(std::string sstring)
{
	if (sstring.empty())
	{
		return "";
	}
	char szBuf1[1024] = {0};
	int nBufLen = sizeof(szBuf1);
	Encrypt((char*)sstring.c_str(), szBuf1, nBufLen);
	return szBuf1;
}
//end
bool GcLoginSession::Login(const char* username, const char* password, char code, bool ishall, bool bForceLogin)
{//1.没有加密的用户名  2.pubwin加密过的用户名   3.是不是pubwin大厅登陆  4.MD5加密后的密码   5.大厅key   6.code = 0
	//2.pubwin加密过的用户名
	std::string pwname = EncryptString(username).c_str();

	//3.是不是pubwin大厅登陆
	char buf1[2];
	memset(buf1,0,sizeof(buf1));
	rt2_sprintf(buf1,"%d",ishall);

	//4.MD5加密后的密码
	//std::string md5Password = GetMD5StringString(password);
	std::string md5Password = "";
	char szMD5[40];
	rtMD5StringString((char*)password, szMD5);
	md5Password = szMD5;
	transform(md5Password.begin(),md5Password.end(),md5Password.begin(),tolower);

	//5.大厅key
	std::string hallkey = "";
	if (ishall)
	{
		 hallkey = password;
	}
	else
	{
		hallkey =  EncryptString(password).c_str();
	}

	CG_CmdPacket& cmd = BeginSend();

	if (!cmd.WriteShort(c2g_login))
		return false;

	if (!cmd.WriteString((char*)username)) //1.没有加密的用户名// 服务器上有检查长度 和 内容，数据库有转意
		return false;

	if (!cmd.WriteString((char*)pwname.c_str()))//2.pubwin加密过的用户名
	{
		return false;
	}

	if (!cmd.WriteString(buf1))//3.是不是pubwin大厅登陆
	{
		return false;
	}

	if (!cmd.WriteString((char*)md5Password.c_str())) // 4.MD5加密后的密码
		return false;

	if (!cmd.WriteString((char*)hallkey.c_str())) // 5.大厅key 服务器上有检查长度 和 内容
		return false;

	if (!cmd.WriteByte(code))//6.code
		return false;

	if(!cmd.WriteByte(bForceLogin))              //0--正常登录,1---强制登录
		return false;

	if (EndSend())
	{
		m_account.account = username;

		return true;
	}

	return false;
}

bool GcLoginSession::CreateChar(const char* name, char metier, char bIsS, char bSex, short headModelID, const char* password, short iAnimal, short iImage)
{
	CG_CmdPacket& cmd = BeginSend();
	//char	md5String[33];

	//rtMD5StringString((char*)password, md5String);
	//md5String[32] = '\0';

	if (!cmd.WriteShort(c2g_create_char))
		return false;

	if (!cmd.WriteString((char*)name)) // 服务器上有检查长度 和 内容，确保不包含特殊字符
		return false;

	if (!cmd.WriteByte(metier))
		return false;

    if (!cmd.WriteByte(bIsS))
        return false;

	if(!cmd.WriteByte(bSex))
		return false;

	if (!cmd.WriteShort(headModelID))
		return false;

	if(!cmd.WriteShort(iAnimal))
		return false;

	if(!cmd.WriteShort(iImage))
		return false;

	return EndSend();
}

bool GcLoginSession::SetCharPassword(long id, const char* password)
{
	CG_CmdPacket& cmd = BeginSend();

	//char	OldMd5String[33];
	//char	NewMd5String[33];

	//rtMD5StringString((char*)old, OldMd5String);
	//OldMd5String[32] = '\0';
	//rtMD5StringString((char*)newpassword, NewMd5String);
	//NewMd5String[32] = '\0';

	if (!cmd.WriteShort(c2g_char_password_set))
	{
		return false;
	}

	if (!cmd.WriteLong(id))
	{
		return false;
	}

	if (!cmd.WriteString((char*)password))
	{
		return false;
	}

	return EndSend();
}

bool GcLoginSession::ChangeCharPassword(long id, const char* old, const char* newpassword)
{
	CG_CmdPacket& cmd = BeginSend();

	//char	OldMd5String[33];
	//char	NewMd5String[33];

	//rtMD5StringString((char*)old, OldMd5String);
	//OldMd5String[32] = '\0';
	//rtMD5StringString((char*)newpassword, NewMd5String);
	//NewMd5String[32] = '\0';

	if (!cmd.WriteShort(c2g_char_password_change))
	{
		return false;
	}

	if (!cmd.WriteLong(id))
	{
		return false;
	}

	if (!cmd.WriteString((char*)old))
	{
		return false;
	}

	if (!cmd.WriteString((char*)newpassword))
	{
		return false;
	}

	return EndSend();
}

bool GcLoginSession::DelCharPassword(long id, const char* password)
{
	CG_CmdPacket& cmd = BeginSend();

	//char	OldMd5String[33];
	//char	NewMd5String[33];

	//rtMD5StringString((char*)old, OldMd5String);
	//OldMd5String[32] = '\0';
	//rtMD5StringString((char*)newpassword, NewMd5String);
	//NewMd5String[32] = '\0';

	if (!cmd.WriteShort(c2g_char_password_del))
	{
		return false;
	}

	if (!cmd.WriteLong(id))
	{
		return false;
	}

	if (!cmd.WriteString((char*)password))
	{
		return false;
	}

	return EndSend();
}

bool GcLoginSession::CharPasswordConfirm(long id, const char* password)
{
	CG_CmdPacket& cmd = BeginSend();

	//char	md5String[33];
	//rtMD5StringString((char*)password, md5String);
	//md5String[32] = '\0';

	if (!cmd.WriteShort(c2g_char_password_confirm))
	{
		return false;
	}

	if (!cmd.WriteLong(id))
	{
		return false;
	}

	if (!cmd.WriteString((char*)password))
	{
		return false;
	}

	return EndSend();
}

bool GcLoginSession::DeleteChar(long id)
{
	CG_CmdPacket& cmd = BeginSend();

	if (!cmd.WriteShort(c2g_delete_char))
		return false;

	if (!cmd.WriteLong(id))
		return false;

	return EndSend();
}

bool GcLoginSession::RestoreChar(long id)
{
	CG_CmdPacket& cmd = BeginSend();

	if (!cmd.WriteShort(c2g_restore_char))
		return false;

	if (!cmd.WriteLong(id))
		return false;

	return EndSend();
}

bool GcLoginSession::QueryAccountInfo()
{
	CG_CmdPacket& cmd = BeginSend();

	if (!cmd.WriteShort(c2g_query_account))
		return false;

	return EndSend();
}

bool GcLoginSession::SelectChar(long id)
{
	CG_CmdPacket& cmd = BeginSend();

	if (!cmd.WriteShort(c2g_select_char))
		return false;

	if (!cmd.WriteLong(id))
		return false;

	return EndSend();
}

void GcLoginSession::RegisterCommand(short id, CommandCallback callback)
{
	m_commands.push_back(Command());
	m_commands.back().id = id;
	m_commands.back().callback = callback;
}

void GcLoginSession::InitChannel()
{
#define REG_CMD(cmd) RegisterCommand(cmd, &gc_net::GcLoginSession::cmd_##cmd);

	REG_CMD(g2c_login_ret);
	REG_CMD(g2c_create_char_ret);
	REG_CMD(g2c_restore_char_ret);
	REG_CMD(g2c_delete_char_ret);
	REG_CMD(g2c_query_account_ret);
    REG_CMD(g2c_select_char_ret);
    REG_CMD(g2c_decline);
    REG_CMD(g2c_session_close);
	REG_CMD(g2c_char_password_change_ret);
	REG_CMD(g2c_char_password_confirm_ret);
	REG_CMD(g2c_char_password_set_ret);
	REG_CMD(g2c_char_password_del_ret);
	REG_CMD(g2c_kick);
}

void GcLoginSession::OnConnect(bool succeeded)
{
	guard;

	LOG1("网络: 连接登陆服务器 [%s]\n", (succeeded?R(G_SUCCEED):R(G_FAIL)));

	GetWorld()->OnNetConnect(succeeded);

	unguard;
}

void GcLoginSession::OnDisconnect()
{
	GetWorld()->OnNetDisconnect();
}

void GcLoginSession::OnReceive(CG_CmdPacket* packet)
{
	short id;

	if (!packet->ReadShort(&id))
		return;
	
	list<Command>::iterator iter = m_commands.begin();

	for (; iter != m_commands.end(); iter++)
	{
		Command& cmd = *iter;

		if (cmd.id == id)
		{
			if (!((this->*cmd.callback)(packet)))
				return;

			break;
		}
	}

	if (iter == m_commands.end())
	{
		LOG1("Unknown command [%d]\n", id);
	}
}

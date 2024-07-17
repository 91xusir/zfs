

#include "core/rt2_core.h"
#include "server_if.h"

bool NES_Initialize_S(unsigned long GameServerID, FP_GM_PUNISHCALLBACK fpGMPunishCallback)
{
	return true;
}
void NES_Finalize_S()
{

}

int NES_UserLogin_S(unsigned long userID, const char* accountID, unsigned long userIP, unsigned char* binBuf, int& length)
{
	return 0;
}
int NES_UserLogout_S(unsigned long userID)
{
	return 0;
}
int NES_Encrypt_S(unsigned long userID, unsigned char* packetbuf, int& buflength, bool isEncrypt, bool isBroadcast)
{
	return 0;
}
int NES_Decrypt_S(unsigned long userID, unsigned char* packetbuf, int& buflength)
{
	return 0;
}


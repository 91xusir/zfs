#ifndef _server_if_h_
#define _server_if_h_
#include "sym_imp.h"
#ifdef __cplusplus
extern "C"
{
#endif
	
//bool NES_Initialize_S(unsigned short GameServerID);
typedef void (*FP_GM_PUNISHCALLBACK)(unsigned int userID, int action, int value);
bool NES_Initialize_S(unsigned long GameServerID, FP_GM_PUNISHCALLBACK fpGMPunishCallback = NULL);
void NES_Finalize_S();
int NES_UserLogin_S(unsigned long userID, const char* accountID, unsigned long userIP, unsigned char* binBuf, int& length);
int NES_UserLogout_S(unsigned long userID);
int NES_Encrypt_S(unsigned long userID, unsigned char* packetbuf, int& buflength, bool isEncrypt = true, bool isBroadcast = false);
int NES_Decrypt_S(unsigned long userID, unsigned char* packetbuf, int& buflength);

#ifdef __cplusplus
}
#endif

#endif

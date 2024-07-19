#ifndef _SYM_IMP_H_
#define _SYM_IMP_H_

#ifdef __cplusplus
extern "C"
{
#endif

unsigned long GM_PacketLength(unsigned char* gamePacket);
unsigned long GM_PacketCommand(unsigned char* gamePacket);
bool GM_IsPacketMustEncrypt(unsigned char* gamePacket);

#ifdef __cplusplus
}
#endif

#endif


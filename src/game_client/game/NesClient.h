#ifndef _NesClient_H
#define _NesClient_H

inline int  NES_Initialize_C(const char* accountID, unsigned int serverIP, unsigned char* binBuf, int length){return 0;};
inline void  NES_Finalize_C(void){};

inline int __stdcall NES_Dispatch_C(int action, unsigned char* packetbuf, int& buflength){return 0;};
#endif
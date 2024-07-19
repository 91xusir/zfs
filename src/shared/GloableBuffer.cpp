
#include "region_client.h"

//GameWorld


//RegionServer
const int c_g_TempBuffSize = 1024 * 512;
G_MEMDEF(g_TempBuff, c_g_TempBuffSize)
G_MEMDEF(s_cmdCompBuf, 32*1024)

#ifdef ENABLE_OPERATOR_NEW

extern G_MEMDEF(g_szExeMD5, 40)
extern G_MEMDEF(g_szDataMD5, 40)
void RegisterMemoryBlock(void)
{
	G_MEMPROTECTOR(g_TempBuff, c_g_TempBuffSize)
	G_MEMPROTECTOR(g_szExeMD5, 40)
	G_MEMPROTECTOR(g_szDataMD5, 40)

	G_MEMPROTECTOR(s_cmdCompBuf, 32*1024)
}

#endif //ENABLE_MEMORY_ALLOCATOR_DEBUG

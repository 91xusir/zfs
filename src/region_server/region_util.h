#ifndef __REGION_UTIL_H__
#define __REGION_UTIL_H__

// 执行客户端上传的命令字符串
const char* DoClientCommand(CRegionCreature *cmdGiver,const char* szCommand, bool bPyGMOP=false, long pyGmAccount=0);

#endif // __REGION_UTIL_H__


#ifndef _STRING_MAPPING_H_
#define _STRING_MAPPING_H_

bool InitMapString(const char* szMapFile);
void ClearMapString();
const char* MapString(const char* szMap);
//-------------add start by tony 05.07.01-----------------------------//
bool InitBannedString(const char* szStrFile);
bool CheckValidName(const char* szName);
bool InitChatBannedString(const char* szStrFile);
bool CheckValidChat(const char* szContent);
//-------------add end   by tony 05.07.01-----------------------------//

/*
void SetTableSearchPath(int iIdx, const char* szPath);
const char* GetTableFileName(const char* szName);
*/

#define R(str)  MapString(#str)
#define Rs(str) MapString(str)
//-------------add start by tony 05.07.01-----------------------------//
#define NAME_IS_VALID(name) CheckValidName(name)
//-------------add end   by tony 05.07.01-----------------------------//

#endif // _STRING_MAPPING_H_

/*----------------------------------------------------------------------------
-   End.
----------------------------------------------------------------------------*/

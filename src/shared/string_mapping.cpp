
#include "string_mapping.h"
#include "core/rt2_core.h"
#include "rt1tort2.h"
#include "linux_util.h"

static HASH_STR(std::string, std::string) s_mapString;
//-------------add start by tony 05.07.01-----------------------------//
static vector<std::string> v_strBanned;
static vector<std::string> v_chatBanned;
//-------------add end   by tony 05.07.01-----------------------------//

//-------------add start by tony 05.07.01-----------------------------//
bool InitChatBannedString(const char* szStrFile)
{
	// HACK,HACK,HACK it is wrong
	GetRightPath((char*)szStrFile);
	v_chatBanned.clear();

	RtCsv csv;
	std::string strValue;

	if(!csv.LoadTableFile(szStrFile))
	{
		ERR1("ERROR: Cannot open banned string file [%s]\n", szStrFile);
		return false;
	}

	int i, iTotal = csv.GetLineCount();
	for(i=0; i<iTotal; i++)
	{
		strValue = csv[i][0].SafeStr();

		v_chatBanned.push_back(strValue);		
	}

	return true;
}

bool CheckValidChat(const char* szContent)
{
	if(szContent == NULL)
	{
		return true;
	}

	for(int i = 0; i < (int)v_chatBanned.size(); i++)
	{
		if (strstr(szContent, v_chatBanned[i].c_str()))
		{
			std::string str=szContent;
			char* charn=(char *)szContent;
			int num=str.find(v_chatBanned[i].c_str());
			std::string schatban=v_chatBanned[i].c_str();
            for (size_t j = num; j < schatban.size() + num; j++)
			{
				charn[j]='*';
			}
			if (i==(int)v_chatBanned.size()-1)
			{
				if (strstr(szContent, v_chatBanned[i].c_str()))
				{
					i=0;
				}else
				{
					return false;
				}	
			}else
			{
				i=0;
			}
			
		}
	}

	return true;
}

bool InitBannedString(const char* szStrFile)
{
	// HACK,HACK,HACK
	GetRightPath((char*)szStrFile);
	v_strBanned.clear();

	RtCsv csv;
	std::string strValue;

	if(!csv.LoadTableFile(szStrFile))
	{
		ERR1("ERROR: Cannot open banned string file [%s]\n", szStrFile);
		return false;
	}

	int i, iTotal = csv.GetLineCount();
	for(i=0; i<iTotal; i++)
	{
		strValue = csv[i][1].SafeStr();

		v_strBanned.push_back(strValue);		
	}

	return true;
}

bool CheckValidName(const char* szName)
{
	if(szName == NULL)
	{
		return false;
	}

	for(int i = 0; i < (int)v_strBanned.size(); i++)
	{
        if (strstr(szName, v_strBanned[i].c_str()))
        {
			return false;
        }
	}

	return true;
}
//-------------add end   by tony 05.07.01-----------------------------//

bool InitMapString(const char* szMapFile)
{
	// HACK,HACK,HACK
	GetRightPath((char*)szMapFile);
    ClearMapString();

    RtCsv csv;
    std::string strKey;
    std::string strValue;

    if(!csv.LoadTableFile(szMapFile))
    {
        ERR1("ERROR: Cannot open string mapping file [%s]\n", szMapFile);
        return false;
    }

    int i, iTotal = csv.GetLineCount();
    for(i=0; i<iTotal; i++)
    {
        strKey = csv[i][0].SafeStr();
        strValue = csv[i][1].SafeStr();

        if (strKey.length())
        {
            if (s_mapString.find(strKey)!=s_mapString.end())
            {
                ERR1("ERROR: string mapping key REDUPLICATE ![%s]\n", szMapFile);
                CHECKEX("string mapping key REDUPLICATE !");
            }

            s_mapString[strKey] = strValue;
        }
    }

    return true;
}

void ClearMapString()
{
    s_mapString.clear();
}

const char* MapString(const char* szMap)
{
    static std::string strKey;
    static HASH_STR(std::string, std::string)::iterator it;
    strKey = szMap;
    it = s_mapString.find(strKey);
    if (it!=s_mapString.end())
    {
        return (*it).second.c_str();
    }

	ERR1("ERROR: Can't find R(%s)\n",szMap);
    return szMap;
}

/*
char m_szSearchPath [4][100] = {"./", "sv_table/", "", ""};
void SetTableSearchPath(int iIdx, const char* szPath)
{
    if (iIdx<0 || iIdx>=4)
    {
        iIdx = 0;
    }
    rt2_strncpy(m_szSearchPath, szPath, 100);
    m_szSearchPath[99] = 0;
}

const char* GetTableFileName(const char* szName)
{
    // ??????????
    int i;
    static char file[MAX_PATH];
    for (i=0; i<4; i++)
    {
        rt2_strcpy(file, m_szSearchPath[i]);
        rt2_strncpy(file+strlen(file), filename, 100-strlen(file));
        file[99] = 0;
        if (RtCoreFile().FileExist(file))
        {
            return file;
        }
    }
    rt2_strcpy(file, "???");
    rt2_strncpy(file+strlen(file), filename, 100-strlen(file));
    file[99] = 0;
    return file;
}
*/

/*----------------------------------------------------------------------------
-   End.
----------------------------------------------------------------------------*/

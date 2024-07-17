
#include "core/rt2_core.h"
#include "rts_scene_area.h"

RtsSceneArea::RtsSceneArea()
{
}

RtsSceneArea::~RtsSceneArea()
{
    RemoveAll();
}

bool RtsSceneArea::LoadFromFile(const char* szFilename)
{
    RtArchive* pFile = RtCoreFile().CreateFileReader(szFilename);
    if (pFile==NULL)
    {
        RtCoreLog().Error("RtsSceneArea: 打开区域信息文件[%s]失败.\n", szFilename);
        return false;
    }

    // 文件信息
    DWORD dwRead;
    DWORD dwFileFlag = 'A'<<24 | 'r'<<16 | 'e'<<8 | 'a';
    *pFile << dwRead;
    if (dwRead!=dwFileFlag)
    {
        RtCoreLog().Error("RtsSceneArea::LoadFromFile: 打开的文件[%s]并非区域信息文件.\n", szFilename);
        RtCoreFile().CloseFile(pFile);
        return false;
    }

    // 版本号
    DWORD dwFileVersion = 1;
    *pFile << dwRead;
    if (dwRead!=dwFileVersion)
    {
        RtCoreLog().Error("RtsSceneArea::LoadFromFile: 打开的区域信息文件[%s]版本[%d]不支持.\n", szFilename, dwRead);
        RtCoreFile().CloseFile(pFile);
        return false;
    }

    RemoveAll();

    int i, iSize, iCount;
    RtsAreaHelper* pArea;
    RtString szName;

    // 文件信息
    *pFile << iCount;
    *pFile << iSize;

    for (i=0; i<iCount; i++)
    {
        *pFile << szName;
        pArea = AddArea(szName);
        *pFile << pArea->lMinX;
        *pFile << pArea->lMinY;
        *pFile << pArea->lMaxX;
        *pFile << pArea->lMaxY;
        *pFile << pArea->szData;
    }

    // 结束标记
    *pFile << dwRead;
    if (dwRead!=dwFileFlag)
    {
        RtCoreLog().Error("RtsSceneArea::LoadFromFile: 打开的区域信息文件[%s]格式错误.\n", szFilename);
        RtCoreFile().CloseFile(pFile);
        return false;
    }
    RtCoreFile().CloseFile(pFile);
    return true;
}

bool RtsSceneArea::SaveToFile(const char* szFilename)
{
    RtArchive* pFile = RtCoreFile().CreateFileWriter(NULL, szFilename);
    if (pFile==NULL)
    {
        RtCoreLog().Error("RtsSceneArea::SaveToFile: 打开区域信息文件[%s]失败.\n", szFilename);
        return false;
    }

    // 取得总共字符串大小
    int iSize=0, iCount;
    RtsAreaHelper* pArea;
    for (m_itCur=m_mapAreas.begin(); m_itCur!=m_mapAreas.end(); m_itCur++)
    {
        pArea = m_itCur->second;
        iSize += pArea->szName.GetLength()+1;
        iSize += pArea->szData.GetLength()+1;
    }

    // 文件信息
    DWORD dwFileFlag = 'A'<<24 | 'r'<<16 | 'e'<<8 | 'a';
    *pFile << dwFileFlag;

    // 版本号
    DWORD dwFileVersion = 1;
    *pFile << dwFileVersion;

    // 文件信息
    iCount = (int)m_mapAreas.size();
    *pFile << iCount;
    *pFile << iSize;

    for (m_itCur=m_mapAreas.begin(); m_itCur!=m_mapAreas.end(); m_itCur++)
    {
        pArea = m_itCur->second;
        *pFile << pArea->szName;
        *pFile << pArea->lMinX;
        *pFile << pArea->lMinY;
        *pFile << pArea->lMaxX;
        *pFile << pArea->lMaxY;
        *pFile << pArea->szData;
    }

    // 结束标记
    *pFile << dwFileFlag;
    RtCoreFile().CloseFile(pFile);

    return true;
}

RtsAreaHelper* RtsSceneArea::AddArea(RtString& szName)
{
    RtsAreaHelper* pNew = RT_NEW RtsAreaHelper;
    pNew->szName = szName;
    pNew->lMinX = 0;
    pNew->lMinY = 0;
    pNew->lMaxX = 0;
    pNew->lMaxY = 0;
    m_mapAreas.insert(HASH_MULTISTR(std::string, RtsAreaHelper*)::value_type(std::string(szName), pNew));
    return pNew;
}

void RtsSceneArea::SetName(RtsAreaHelper* pArea, const char* szName)
{
    if (_RemoveArea(pArea, false))
    {
        pArea->szName = szName;
        m_mapAreas.insert(HASH_MULTISTR(std::string, RtsAreaHelper*)::value_type(std::string(szName), pArea));
    }
}

bool RtsSceneArea::RemoveArea(RtsAreaHelper* pArea)
{
    return _RemoveArea(pArea, true);
}

bool RtsSceneArea::_RemoveArea(RtsAreaHelper* pArea, bool bDelete)
{
    for (m_itCur=m_mapAreas.begin(); m_itCur!=m_mapAreas.end(); m_itCur++)
    {
        if (pArea==(m_itCur->second))
        {
            m_mapAreas.erase(m_itCur);
            if (bDelete) DEL_ONE(pArea);
            return true;
        }
    }
    return false;
}

bool RtsSceneArea::RemoveArea(const char* szName)
{
    m_itCur = m_mapAreas.find(std::string(szName));
    if (m_itCur==m_mapAreas.end())
    {
        return false;
    }
    DEL_ONE(m_itCur->second);
    m_mapAreas.erase(m_itCur);
    return true;
}

void RtsSceneArea::RemoveAll()
{
    for (m_itCur=m_mapAreas.begin(); m_itCur!=m_mapAreas.end(); m_itCur++)
    {
        DEL_ONE( m_itCur->second );
    }
    m_mapAreas.clear();
}

RtsAreaHelper* RtsSceneArea::GetFirst()
{
    m_itCur = m_mapAreas.begin();
    if (m_itCur==m_mapAreas.end())
    {
        return NULL;
    }
    return m_itCur->second;
}

RtsAreaHelper* RtsSceneArea::GetNext()
{
    m_itCur ++;
    if (m_itCur==m_mapAreas.end())
    {
        return NULL;
    }
    return m_itCur->second;
}

RtsAreaHelper* RtsSceneArea::FindArea(const char* szName)
{
    m_itCur = m_mapAreas.find(std::string(szName));
    if (m_itCur==m_mapAreas.end())
    {
        return NULL;
    }
    return m_itCur->second;

	//std::string a;
	//HASH_MULTISTR(std::string, RtsAreaHelper*)::iterator it;
	//for(it=m_mapAreas.begin();it!=m_mapAreas.end();it++)
	//{
	//	a=it->first;
	//	if(a==szName)
	//	{
	//		return it->second;
	//	}
	//}
	//return NULL;
}



std::string RtsSceneArea::FindArea(int x,int y)
{
	RtsAreaHelper* AreaHelper;
	HASH_MULTISTR(std::string, RtsAreaHelper*)::iterator it;
	for(it=m_mapAreas.begin();it!=m_mapAreas.end();it++)
	{
		AreaHelper=it->second;
		if(x>AreaHelper->lMinX && x<AreaHelper->lMaxX && y>AreaHelper->lMinY && y<AreaHelper->lMaxY)
		{
			return it->first;
		}
	}
	return "";
}
/*------------------------------------------------------------------------
-   The End.
------------------------------------------------------------------------*/

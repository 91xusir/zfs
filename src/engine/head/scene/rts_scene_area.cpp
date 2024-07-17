
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
        RtCoreLog().Error("RtsSceneArea: ��������Ϣ�ļ�[%s]ʧ��.\n", szFilename);
        return false;
    }

    // �ļ���Ϣ
    DWORD dwRead;
    DWORD dwFileFlag = 'A'<<24 | 'r'<<16 | 'e'<<8 | 'a';
    *pFile << dwRead;
    if (dwRead!=dwFileFlag)
    {
        RtCoreLog().Error("RtsSceneArea::LoadFromFile: �򿪵��ļ�[%s]����������Ϣ�ļ�.\n", szFilename);
        RtCoreFile().CloseFile(pFile);
        return false;
    }

    // �汾��
    DWORD dwFileVersion = 1;
    *pFile << dwRead;
    if (dwRead!=dwFileVersion)
    {
        RtCoreLog().Error("RtsSceneArea::LoadFromFile: �򿪵�������Ϣ�ļ�[%s]�汾[%d]��֧��.\n", szFilename, dwRead);
        RtCoreFile().CloseFile(pFile);
        return false;
    }

    RemoveAll();

    int i, iSize, iCount;
    RtsAreaHelper* pArea;
    RtString szName;

    // �ļ���Ϣ
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

    // �������
    *pFile << dwRead;
    if (dwRead!=dwFileFlag)
    {
        RtCoreLog().Error("RtsSceneArea::LoadFromFile: �򿪵�������Ϣ�ļ�[%s]��ʽ����.\n", szFilename);
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
        RtCoreLog().Error("RtsSceneArea::SaveToFile: ��������Ϣ�ļ�[%s]ʧ��.\n", szFilename);
        return false;
    }

    // ȡ���ܹ��ַ�����С
    int iSize=0, iCount;
    RtsAreaHelper* pArea;
    for (m_itCur=m_mapAreas.begin(); m_itCur!=m_mapAreas.end(); m_itCur++)
    {
        pArea = m_itCur->second;
        iSize += pArea->szName.GetLength()+1;
        iSize += pArea->szData.GetLength()+1;
    }

    // �ļ���Ϣ
    DWORD dwFileFlag = 'A'<<24 | 'r'<<16 | 'e'<<8 | 'a';
    *pFile << dwFileFlag;

    // �汾��
    DWORD dwFileVersion = 1;
    *pFile << dwFileVersion;

    // �ļ���Ϣ
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

    // �������
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

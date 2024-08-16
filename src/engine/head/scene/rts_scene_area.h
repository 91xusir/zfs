#include <unordered_map>

#ifndef __rts_scene_area_H__
#define __rts_scene_area_H__

using namespace EXT_SPACE;
class RtsSceneArea;

struct RtsAreaHelper {
    long     lMinX;  // ����
    long     lMinY;
    long     lMaxX;
    long     lMaxY;
    RtString szData;  // �ַ�������

    friend class RtsSceneArea;

    const RtString& GetName() { return szName; }

   private:
    RtString szName;  // ����(Ψһ��)
};

class RtsSceneArea {
   public:
    RtsSceneArea();
    virtual ~RtsSceneArea();

    bool LoadFromFile(const char* szFilename);  // ���ļ�����������Ϣ
    bool SaveToFile(const char* szFilename);    // ��������Ϣ�浽�ļ���

    int GetCount() { return (int)m_mapAreas.size(); }

    RtsAreaHelper* GetFirst();
    RtsAreaHelper* GetNext();
    RtsAreaHelper* FindArea(const char* szName);
    std::string    FindArea(int x, int y);
    void           SetName(RtsAreaHelper* pArea, const char* szName);

    RtsAreaHelper* AddArea(RtString& szName);
    bool           RemoveArea(RtsAreaHelper* pArea);
    bool           RemoveArea(const char* szName);
    void           RemoveAll();

   private:
    bool _RemoveArea(RtsAreaHelper* pArea, bool bDelete);
    std::unordered_multimap<std::string, RtsAreaHelper*>           m_mapAreas;
    std::unordered_multimap<std::string, RtsAreaHelper*>::iterator m_itCur;
};

#endif

/*------------------------------------------------------------------------
-   The End.
------------------------------------------------------------------------*/

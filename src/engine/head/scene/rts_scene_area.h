#include <unordered_map>

#ifndef __rts_scene_area_H__
#define __rts_scene_area_H__

using namespace EXT_SPACE;
class RtsSceneArea;

struct RtsAreaHelper {
    long     lMinX;  // 区域
    long     lMinY;
    long     lMaxX;
    long     lMaxY;
    RtString szData;  // 字符串数据

    friend class RtsSceneArea;

    const RtString& GetName() { return szName; }

   private:
    RtString szName;  // 名字(唯一的)
};

class RtsSceneArea {
   public:
    RtsSceneArea();
    virtual ~RtsSceneArea();

    bool LoadFromFile(const char* szFilename);  // 从文件载入区域信息
    bool SaveToFile(const char* szFilename);    // 把区域信息存到文件中

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


/*------------------------------------------------------------------------
-   Class CXmlFileMapTexture.
------------------------------------------------------------------------*/
// CXmlFileMapTexture 类继承自 RtScriptXML 类，用于解析 XML 文件中与纹理映射相关的节点和属性
class CXmlFileMapTexture : public RtScriptXML
{
public:
    // 构造函数，初始化成员变量
    CXmlFileMapTexture(RtScene* pScene)
    {
        m_pScene = pScene; // 将传入的场景指针保存到成员变量
        m_iNodeType = 0;   // 初始化节点类型为 0
    }

    // 虚析构函数，确保派生类的资源能够正确释放
    virtual ~CXmlFileMapTexture()
    {
    }

    // 错误处理函数，在 XML 解析过程中发生错误时调用
    virtual void OnError(const char* szFilename, const char* szErrMsg, long lLineNo)
    {
        // 记录错误信息，包括错误消息、文件名和行号
        RtCoreLog().Error("XML ERROR: \"%s\", File '%s', line %d\n", szErrMsg, szFilename, lLineNo);
        // 抛出错误异常
        CHECKEX("XML ERROR");
    }

    // XML 标签开始处理函数，当解析器遇到标签的开始部分时调用
    virtual void OnTagStart(const char *pTag)
    {
        // 如果标签是 "map"，将节点类型设置为 1
        if (stricmp(pTag, "map") == 0)
        {
            m_iNodeType = 1;
        }
        // 如果标签是 "scene_map_texture_list"，将节点类型设置为 0
        else if (stricmp(pTag, "scene_map_texture_list") == 0)
        {
            m_iNodeType = 0;
        }
        // 其他未知标签，记录错误并抛出异常
        else
        {
            m_iNodeType = 0;
            RtCoreLog().Error("XML ERROR: Unknown Tag \"%s\"\n", pTag);
            CHECKEX("XML ERROR");
        }
        // 重置属性计数器、索引、类型以及名称和文件名字符串
        m_iAttrCnt = 0;
        m_iIdx = 0;
        m_iType = 0;
        m_strName = "";
        m_strFile = "";
    }

    // XML 属性处理函数，当解析器遇到标签的属性时调用
    virtual void OnAttribute(const char *pAttribute, const char *pValue)
    {
        // 如果当前节点不是 "map" 节点，直接返回
        if (m_iNodeType != 1) return;

        // 处理不同的属性名称，根据属性名称设置相应的成员变量
        if (stricmp(pAttribute, "name") == 0)
        {
            m_strName = pValue; // 保存纹理的名称
        }
        else if (stricmp(pAttribute, "idx") == 0)
        {
            m_iIdx = atoi(pValue); // 将索引字符串转换为整数并保存
        }
        else if (stricmp(pAttribute, "type") == 0)
        {
            m_iType = atoi(pValue); // 将类型字符串转换为整数并保存
        }
        else if (stricmp(pAttribute, "file") == 0)
        {
            m_strFile = pValue; // 保存纹理文件的名称
        }
        // 处理未知属性，记录错误并抛出异常
        else
        {
            RtCoreLog().Error("XML ERROR: Unknown Attribute \"%s\"\n", pAttribute);
            CHECKEX("XML ERROR");
        }

        // 增加属性计数器
        m_iAttrCnt++;
    }

    // XML 标签结束处理函数，当解析器遇到标签的结束部分时调用
    virtual void OnTagEnd()
    {
        // 如果当前节点不是 "map" 节点，直接返回
        if (m_iNodeType != 1) return;

        // 检查索引和属性数量是否符合要求，确保数据正确
        CHECK(m_iIdx >= 0 && m_iIdx < RtScene::MAX_MAP_COUNT && m_iAttrCnt == 4);

        // 将解析到的纹理信息保存到场景对象的纹理映射数组中
        m_pScene->m_texMaps[m_iIdx].szName = m_strName;
        m_pScene->m_texMaps[m_iIdx].szFile = m_strFile;
        m_pScene->m_texMaps[m_iIdx].iType = m_iType;
        m_pScene->m_texMaps[m_iIdx].texItem = NULL; // 初始化纹理项为空
    }

    // XML 标签立即结束处理函数，与 OnTagEnd 相同
    virtual void OnTagEndImme() { OnTagEnd(); }

private:
    RtScene*    m_pScene;    // 场景指针，用于访问场景的纹理映射数据

    int         m_iNodeType; // 当前节点类型
    int         m_iAttrCnt;  // 属性计数器
    int         m_iIdx;      // 纹理映射的索引
    int         m_iType;     // 纹理的类型
    RtString    m_strName;   // 纹理的名称
    RtString    m_strFile;   // 纹理文件的名称
};

/*------------------------------------------------------------------------
-   Class CXmlFileLightSet.
------------------------------------------------------------------------*/

class CXmlFileLightSet : public RtScriptXML
{
public:
    CXmlFileLightSet(RtScene* pScene)
    {
        m_pScene=pScene;
        m_iNodeType = 0;
        m_pLightSet = NULL;
    }
    virtual ~CXmlFileLightSet()
    {
        if (m_pLightSet)
        {
            DEL_ONE(m_pLightSet);
        }
    }
    virtual void OnError(const char* szFilename, const char* szErrMsg, long lLineNo)
    {
        RtCoreLog().Error("XML ERROR: \"%s\", File '%s', line %d\n", szErrMsg, szFilename, lLineNo);
        CHECKEX("XML ERROR");
    }
    virtual void OnTagStart(const char *pTag)
    {   // <pTag
        if (stricmp(pTag, "lightset")==0)
        {
            m_iNodeType = 1;
        }else if (stricmp(pTag, "key")==0)
        {
            m_iNodeType = 2;
            iCnt = 0;
        }else if (stricmp(pTag, "sun_color")==0)
        {
            m_iNodeType = 3;
            iCnt ++;
        }else if (stricmp(pTag, "sun_specular")==0)
        {
            m_iNodeType = 4;
            iCnt ++;
        }else if (stricmp(pTag, "ambient_color")==0)
        {
            m_iNodeType = 5;
            iCnt ++;
        }else if (stricmp(pTag, "terrain_ambient_color")==0)
        {
            m_iNodeType = 6;
            iCnt ++;
        }else if (stricmp(pTag, "scene_lightset")==0)
        {
            m_iNodeType = 0;
        }else
        {
            m_iNodeType =0;
            RtCoreLog().Error("XML ERROR: Unknown Tag \"%s\"\n", pTag);
            CHECKEX("XML ERROR");
        }
        i1 = i2 = i3 = 0;
        m_str = "";
    }
    virtual void OnAttribute(const char *pAttribute, const char *pValue)
    {   // pAttribute=pValue
        if (m_iNodeType==0) return;
        if (stricmp(pAttribute, "name")==0)
        {
            m_str = pValue;
        }else if (stricmp(pAttribute, "time")==0)
        {
            i1 = atoi(pValue);
        }else if (stricmp(pAttribute, "cr")==0)
        {
            i1 = atof(pValue);
        }else if (stricmp(pAttribute, "cg")==0)
        {
            i2 = atof(pValue);
        }else if (stricmp(pAttribute, "cb")==0)
        {
            i3 = atof(pValue);
        }else
        {
            RtCoreLog().Error("XML ERROR: Unknown Attribute \"%s\"\n", pAttribute);
            CHECKEX("XML ERROR");
        }
    }
    virtual void OnTagEnd()
    {   // >
        if (m_iNodeType==0) return;
        switch (m_iNodeType)
        {
        case 1: // 一个 lightset 结束
            if (m_str=="default")
            {
                m_pLightSet = &(m_pScene->m_lightSet);
            }else
            {
                m_pLightSet = RT_NEW RtsSLightSet;
                m_pLightSet->pNext = m_pScene->m_lightSet.pNext;
                m_pScene->m_lightSet.pNext = m_pLightSet;
                m_pLightSet->iEnumCnt = 0;
                m_pLightSet->szName = m_str;
            }
            break;
        case 2: // 一个key结束
            m_LightEnum.iHour = i1;
            break;
        case 3: // sun_color
            m_LightEnum.vSunColor.x = i1/255.f;
            m_LightEnum.vSunColor.y = i2/255.f;
            m_LightEnum.vSunColor.z = i3/255.f;
            break;
        case 4: // sun_specular
            m_LightEnum.vSunSpecular.x = i1/255.f;
            m_LightEnum.vSunSpecular.y = i2/255.f;
            m_LightEnum.vSunSpecular.z = i3/255.f;
            break;
        case 5: // ambient_color
            m_LightEnum.vAmbientColor.x = i1/255.f;
            m_LightEnum.vAmbientColor.y = i2/255.f;
            m_LightEnum.vAmbientColor.z = i3/255.f;
            break;
        case 6: // terrain_ambient_color
            m_LightEnum.vTerrainAmbientColor.x = i1/255.f;
            m_LightEnum.vTerrainAmbientColor.y = i2/255.f;
            m_LightEnum.vTerrainAmbientColor.z = i3/255.f;
            break;
        }
    }
    virtual void OnCloseTagStart(const char *pTag)
    {
        if (stricmp(pTag, "lightset")==0)
        {
            m_pLightSet = NULL;
        }else if (stricmp(pTag, "key")==0)
        {
            if (iCnt!=4)
            {
                RtCoreLog().Error("XML错误: 灯光设置的属性个数不对\n");
                CHECKEX("XML ERROR");
                return;
            }
            if (m_pLightSet->iEnumCnt>=10)
            {
                RtCoreLog().Error("XML错误: 一个LightSet里面最多只能有10个字段\n");
                CHECKEX("XML ERROR");
                return;
            }
            m_pLightSet->pEnums[m_pLightSet->iEnumCnt] = m_LightEnum;
            m_pLightSet->iEnumCnt ++;
        }
    }
    virtual void OnTagEndImme() { OnTagEnd(); }
private:
    RtScene*    m_pScene;
    int         m_iNodeType;

    RtsSLightSet*     m_pLightSet;
    RtsSLightSetEnum  m_LightEnum;
    RtString                m_str;
    int                     i1, i2, i3, iCnt;
};

/*------------------------------------------------------------------------
-   Class CXmlFileWater.
------------------------------------------------------------------------*/

class CXmlFileWater : public RtScriptXML
{
public:
    CXmlFileWater(RtScene* pScene)
    {
        m_pScene=pScene;
        m_iNodeType = 0;
    }
    virtual ~CXmlFileWater()
    {
    }
    virtual void OnError(const char* szFilename, const char* szErrMsg, long lLineNo)
    {
        RtCoreLog().Error("XML ERROR: \"%s\", File '%s', line %d\n", szErrMsg, szFilename, lLineNo);
        CHECKEX("XML ERROR");
    }
    virtual void OnTagStart(const char *pTag)
    {   // <pTag
        if (stricmp(pTag, "map")==0)
        {
            m_iNodeType = 1;
        }else if (stricmp(pTag, "scene_map_water")==0)
        {
            m_iNodeType = 0;
        }else
        {
            m_iNodeType = 0;
            RtCoreLog().Error("XML ERROR: Unknown Tag \"%s\"\n", pTag);
            CHECKEX("XML ERROR");
        }
        m_iAttrCnt = 0;
        m_iIdx = 0;
        m_strFile = "";
        m_iAlpha = 0;
        m_iDepth = 0;
    }
    virtual void OnAttribute(const char *pAttribute, const char *pValue)
    {   // pAttribute=pValue
        if (m_iNodeType!=1) return;
        if (stricmp(pAttribute, "idx")==0)
        {
            m_iIdx = atoi(pValue);
        }else if (stricmp(pAttribute, "name")==0)
        {
            m_strName = pValue;
        }else if (stricmp(pAttribute, "file")==0)
        {
            m_strFile = pValue;
        }else if (stricmp(pAttribute, "alpha")==0)
        {
            m_iAlpha = atoi(pValue);
        }else if (stricmp(pAttribute, "depth")==0)
        {
            m_iDepth = atoi(pValue);
        }else if (stricmp(pAttribute, "r")==0)
        {
            m_iColorR = atoi(pValue);
        }else if (stricmp(pAttribute, "g")==0)
        {
            m_iColorG = atoi(pValue);
        }else if (stricmp(pAttribute, "b")==0)
        {
            m_iColorB = atoi(pValue);
        }else if (stricmp(pAttribute, "colorrate")==0)
        {
            m_fColorRate = atof(pValue);
        }else
        {
            RtCoreLog().Error("XML ERROR: Unknown Attribute \"%s\"\n", pAttribute);
        }
        m_iAttrCnt ++;
    }
    virtual void OnTagEnd()
    {   // >
        if (m_iNodeType!=1) return;
        CHECK(m_iIdx>=0 && m_iIdx<RtScene::MAX_WATER_COUNT && m_iAttrCnt==9);
        m_pScene->m_Waters[m_iIdx].szName = m_strName;
        m_pScene->m_Waters[m_iIdx].szFile = m_strFile;
        m_pScene->m_Waters[m_iIdx].iAlpha = m_iAlpha;
        m_pScene->m_Waters[m_iIdx].iDepth = m_iDepth;
        m_pScene->m_Waters[m_iIdx].dwColor = ((m_iColorR&0x0FF)<<16) | ((m_iColorG&0x0FF)<<8) | (m_iColorB&0x0FF);
        m_pScene->m_Waters[m_iIdx].fColorRate = m_fColorRate;
    }
    virtual void OnTagEndImme() { OnTagEnd(); }
private:
    RtScene*    m_pScene;

    int         m_iNodeType;
    int         m_iAttrCnt;

    int         m_iIdx;
    RtString    m_strName;
    RtString    m_strFile;
    int         m_iAlpha;
    int         m_iDepth;
    int         m_iColorR;
    int         m_iColorG;
    int         m_iColorB;
    float       m_fColorRate;
};

/*------------------------------------------------------------------------
-   The End.
------------------------------------------------------------------------*/

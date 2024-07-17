


class RtSceneBlock;

#define   MAX_VERSION   1
#define   MIN_VERSION   0

#define   OBJECT_VERSION         ( MAX_VERSION << 16 ) | ( MIN_VERSION )
#define   GET_MAX_VERSION        ( ( ( OBJECT_VERSION & 0xffff0000 ) >> 8 ) & 0x0000ffff )
#define   GET_MIN_VERSION        ( ( OBJECT_VERSION ) & 0x0000ffff )

class RtsObject : public RtObject
{
    RT_DECLARE_DYNAMIC(RtsObject, RtObject, 0, "scene")
    friend class RtScene;
    friend class RtsObjectManager;
    friend class RtSceneBlock;

public :

    enum EShowObject
    {
        SO_NONE,       
        SO_SPRITE,      
        SO_BOX,        
        SO_MODEL
    };

    enum EObjectSharp
    {
        EOS_HUGEOBJECT = 0x01,
        EOS_BIGOBJECT ,
        EOS_MEDIAOBJECT,
        EOS_SMALLOBJECT,
        EOS_UNKNOWNOBJECT
    };

    enum
    {
        LEVEL_HUGE  = 500,
        LEVEL_BIG   = 300,
        LEVEL_MEDIA = 100,
        LEVEL_SMALL = 80
    };

protected :
    RtsObject();
    virtual ~RtsObject();
    
public :
    virtual long Serialize(RtArchive& ar);
    virtual void FrameMove(float _timeMill);
    virtual void Render(RtgCamera& inCamera, RtgDevice& inDevice, RTGRenderMask eMask);
    virtual void RenderHighLight(RtgCamera& inCamera, RtgDevice& inDevice, RtgVertex3 &color,RtgVertex3 &scale);
    
    void GetGridRange(int& iMinX, int& iMinY, int& iMaxX, int& iMaxY);
    void GetTileRange(int& iMinX, int& iMinY, int& iMaxX, int& iMaxY);
	void GetTerrainTileRange(int& iMinX, int& iMinY, int& iMaxX, int& iMaxY);

    virtual const RtgAABB* Get_AABBox() const = 0;
    virtual const RtgAABB* Get_World_AABBox() const = 0;

    virtual void ResetPosition()
    {
    }

    virtual void SetMatrix(RtgMatrix16& vM16) 
    { 
        m_Matrix = vM16; 
        OnMatrixChanged(); 
    }

    RtgMatrix16& GetMatrix() 
    { 
        return m_Matrix; 
    }
    
    virtual void OnMatrixChanged()
    {
    }

    virtual bool CreateResource(const char fileName[], RtsSGrid* _grid, bool bForceLoad)
    { 
        m_fileName = fileName;
        SetState(Object_State_Ready);
        return true;
    }

    virtual void ReleaseResource() 
    {
        SetState(Object_State_New);
    }

    virtual bool RecreateResource(RtsSGrid* _grid, bool bForceLoad = false) 
    { 
        ReleaseResource(); 
        return CreateResource(m_fileName.c_str(), _grid, bForceLoad); 
    }

    virtual bool DataVaild() const
    {
        return true;
    }

    void SetId(objectId _id)
    {
        m_Id = _id;
    }

    objectId Id() const
    {
        return m_Id;
    }

    void SetLastRenderFrame(UINT _frame)
    {
        m_uLastRenderFrame = _frame;
    }

    UINT GetLastRenderFrame() const
    {
        return m_uLastRenderFrame;
    }

public :
    objectId        m_Id;
    float           m_fRenderDistance;     
    RtsObject*      m_pObjectLink;        
    UINT            m_uRef;         
	float           m_fBlendFactor;
	float           m_fRadius;
	EObjectSharp    m_eObjectSharp;
	unsigned long   m_lObjectVersion;
	bool            m_bLastAttribFlag;
    RtgMatrix16     m_Matrix;     
    UINT            m_uLastRenderFrame;
    string          m_fileName;

#if RTS_DRAW_HELPER
    void SetShowType(RtgDevice* pDevice, EShowObject eType);
    EShowObject GetShowType();

    EShowObject     m_eShowType;           
    RtgGeometry     m_Geometry;           
    RtgShader       m_Shader;              
#endif

};

inline void Compatible_Serialize_ObjId(RtArchive& ar, objectId& _id)
{
    if (ar.IsLoading())
    {
        unsigned int t = 0;

        ar<<t;

        if (t == 0xffffffff)
        {
            ar<<_id.data0
              <<_id.data1
              <<_id.data2
              <<_id.data3;
        }
        else
            _id.data0 = t;
    }
    else
    {
        unsigned int t = 0xffffffff;

        ar<<t
          <<_id.data0
          <<_id.data1
          <<_id.data2
          <<_id.data3;
    }
}
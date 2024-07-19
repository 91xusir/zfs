
/*------------------------------------------------------------------------
-   Class RtsRenderAgent.
------------------------------------------------------------------------*/

// 场景物件基类

class RtSceneBlock;

class RtsRenderAgent : public RtsObject
{
    RT_DECLARE_DYNAMIC(RtsRenderAgent, RtsObject, 0, "scene")
protected:
    RtsRenderAgent();
    virtual ~RtsRenderAgent();

public:
    virtual const RtgAABB* Get_AABBox() const
    {
        static RtgAABB _aabox = RtgAABB(RtgVertex3(0.f, 0.f, 0.f), RtgVertex3(10.f, 10.f, 10.f));
        return &_aabox;
    }

    virtual const RtgAABB* Get_World_AABBox() const 
    {
        static RtgAABB _aabox = RtgAABB(RtgVertex3(0.f, 0.f, 0.f), RtgVertex3(10.f, 10.f, 10.f));
        _aabox.vPos = *(RtgVertex3*)&m_Matrix._30;
        return &_aabox;
    }

};

/*------------------------------------------------------------------------
-   class RtsRenderAgentShadow.
------------------------------------------------------------------------*/

class RtsRenderAgentShadow : public RtsActor
{
    RT_DECLARE_DYNAMIC(RtsRenderAgentShadow, RtsActor, 0, "scene")
protected:
    RtsRenderAgentShadow();
    virtual ~RtsRenderAgentShadow();

public:
    //void SetScene(RtSceneBlock* pScene)  { m_pScene = pScene; }
    //RtSceneBlock* GetScene()             { return m_pScene; }
    virtual void AgentRenderShader(){}

};

/*------------------------------------------------------------------------
-   The End.
------------------------------------------------------------------------*/


namespace rt_graph {

class RtgNode
{
public:
    RtgNode(void);
    virtual ~RtgNode();
    virtual void Clone(RtgNode *pNode);

    //////////////////////////////////////////////////////////////////////////
    // 矩阵

    RtgMatrix16 *GetMatrix(void);
    void Set4X3Matrix(float *m12);
    void SetMatrix(RtgMatrix16 &matrix);
    void Identity(void);
    void Translate(float x, float y, float z, int bLocal=1);
    void Rotate(int iAxis, float fDeg, int bLocal=1);
    void Rotate(float fDeg, float x, float y, float z, int bLocal=1);
    void Quaternion(float w, float x, float y, float z, int bLocal=1);

    //////////////////////////////////////////////////////////////////////////
    // 父子结构

    void AddChildren(RtgNode *pNode, bool bAutoDel=true);
    void DeleteChildren(RtgNode *pNode);
    void ClearChildren(void);
    void SetParent(RtgNode *pNode, bool bAutoDel=true);

    inline RtgNode* FirstChild()    { m_pCurrentChildren=m_pFirstChildren; return m_pCurrentChildren;}
    inline RtgNode* NextChild()     { m_pCurrentChildren=m_pCurrentChildren->m_pParent; return m_pCurrentChildren;}
    inline int  ChildrenCount()     { return m_iChildrenCount;}
    inline bool IsAutoDelByParent() { return m_bDelByParent;}
    inline bool IsNeedUpdate()      { return m_bNeedUpdate;}

    void UpdateNodeMatrix(void);
    void UpdateChilrenMatrix(void);

    void Output();

    static int QueryNodeCount(); // 查询当前所有的Node总数, 在程序退出的时候做这个判断可以断言是否有内存泄漏

protected:
    // 属性
    bool m_bAddToScene;
    bool m_bNeedUpdate;
    bool m_bDelByParent;

    // 矩阵
    RtgMatrix16 m_LocalMatrix;
    RtgMatrix16 m_Matrix;

    // 父子结构
    RtgNode* m_pParent;             // 父亲节点
    RtgNode* m_pNextBrother;        // 下一个兄弟节点
    RtgNode* m_pPrevBrother;        // 前一个兄弟节点
    RtgNode* m_pFirstChildren;      // 第一个孩子节点
    RtgNode* m_pLastChildren;       // 最后一个孩子节点
    RtgNode* m_pCurrentChildren;    // 当前的孩子节点 (一些操作的中间变量)
    int      m_iChildrenCount;      // 孩子数

    static int s_iNodeCount;
#ifdef _DEBUG
    RtgNode* m_pNodeListNext;
    RtgNode* m_pNodeListPrev;
    static RtgNode* m_pNodeList;
#endif
};

} // namespace rt_graph

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/

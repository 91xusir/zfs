
namespace rt_graph {

class RtgNode
{
public:
    RtgNode(void);
    virtual ~RtgNode();
    virtual void Clone(RtgNode *pNode);

    //////////////////////////////////////////////////////////////////////////
    // ����

    RtgMatrix16 *GetMatrix(void);
    void Set4X3Matrix(float *m12);
    void SetMatrix(RtgMatrix16 &matrix);
    void Identity(void);
    void Translate(float x, float y, float z, int bLocal=1);
    void Rotate(int iAxis, float fDeg, int bLocal=1);
    void Rotate(float fDeg, float x, float y, float z, int bLocal=1);
    void Quaternion(float w, float x, float y, float z, int bLocal=1);

    //////////////////////////////////////////////////////////////////////////
    // ���ӽṹ

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

    static int QueryNodeCount(); // ��ѯ��ǰ���е�Node����, �ڳ����˳���ʱ��������жϿ��Զ����Ƿ����ڴ�й©

protected:
    // ����
    bool m_bAddToScene;
    bool m_bNeedUpdate;
    bool m_bDelByParent;

    // ����
    RtgMatrix16 m_LocalMatrix;
    RtgMatrix16 m_Matrix;

    // ���ӽṹ
    RtgNode* m_pParent;             // ���׽ڵ�
    RtgNode* m_pNextBrother;        // ��һ���ֵܽڵ�
    RtgNode* m_pPrevBrother;        // ǰһ���ֵܽڵ�
    RtgNode* m_pFirstChildren;      // ��һ�����ӽڵ�
    RtgNode* m_pLastChildren;       // ���һ�����ӽڵ�
    RtgNode* m_pCurrentChildren;    // ��ǰ�ĺ��ӽڵ� (һЩ�������м����)
    int      m_iChildrenCount;      // ������

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

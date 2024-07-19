#include "rtg_graph_inter.h"

namespace rt_graph {
int RtgNode::s_iNodeCount = 0;
#ifdef _DEBUG
RtgNode* RtgNode::m_pNodeList = NULL;
#endif

//////////////////////////////////////////////////////////////////////////
// RtgNode

RtgNode::RtgNode(void)
{
    m_pParent = NULL;
    m_LocalMatrix.Identity();
    m_Matrix.Identity();
    m_bNeedUpdate = false;
    m_bAddToScene = false;
    m_bDelByParent = false;

    m_pParent = NULL;             // 父亲节点
    m_pNextBrother = NULL;        // 下一个兄弟节点
    m_pPrevBrother = NULL;        // 前一个兄弟节点
    m_pFirstChildren = NULL;      // 第一个孩子节点
    m_pLastChildren = NULL;       // 最后一个孩子节点
    m_pCurrentChildren = NULL;    // 当前的孩子节点 (一些操作的中间变量)
    m_iChildrenCount = 0;         // 孩子数

    s_iNodeCount ++;
#ifdef _DEBUG
    m_pNodeListNext = NULL;
    m_pNodeListPrev = NULL;

    m_pNodeListNext = m_pNodeList;
    m_pNodeListPrev = NULL;
    m_pNodeList = this;
#endif
}

RtgNode::~RtgNode(void)
{
#ifdef _DEBUG
    if (m_pNodeListNext) m_pNodeListNext->m_pNodeListPrev = m_pNodeListPrev;
    if (m_pNodeListPrev) m_pNodeListPrev->m_pNodeListNext = m_pNodeListNext;
    else                 m_pNodeList = m_pNodeListNext;
#endif
    s_iNodeCount --;

    RtgNode *pNode, *pDel;
    pNode = m_pFirstChildren;
    while (pNode)
    {
        pDel = pNode;
        pNode = pNode->m_pNextBrother;
        if (pDel->m_bDelByParent)
        {
            pDel->SetParent(NULL);
            DEL_ONE(pDel);
        }
    }

    SetParent(NULL);
}

int RtgNode::QueryNodeCount()
{
    return s_iNodeCount;
}

RtgMatrix16* RtgNode::GetMatrix(void)
{
    return &m_LocalMatrix;
}

void RtgNode::UpdateNodeMatrix(void)
{
    if ( m_pParent )
    {
        m_pParent->UpdateNodeMatrix();
        m_Matrix = m_LocalMatrix * m_pParent->m_Matrix;
    }else
    {
        m_Matrix = m_LocalMatrix;
    }
    m_bNeedUpdate = false;
}

void RtgNode::Identity(void)
{
    m_LocalMatrix.Identity();
}

void RtgNode::Set4X3Matrix(float *m12)
{
    m_bNeedUpdate = true;
    m_LocalMatrix.Set4X3Matrix(m12);
}

void RtgNode::SetMatrix(RtgMatrix16 &matrix)
{
    m_bNeedUpdate = true;
    m_LocalMatrix = matrix;
}

void RtgNode::Translate(float x, float y, float z, int bLocal)
{
    if ( bLocal )
    {
        m_LocalMatrix.Translate(x,y,z);
    }else
    {
        RtgMatrix16 m;
        m.Identity();
        m.Translate(x,y,z);
        m_LocalMatrix = m_LocalMatrix * m;
    }
    m_bNeedUpdate = true;
}

void RtgNode::Rotate(int iAxis, float fDeg, int bLocal)
{
    RtgMatrix16 m;
    m.Identity();

    switch(iAxis)
    {
    case 0:     m.RotateX(fDeg); break;
    case 1:     m.RotateY(fDeg); break;
    case 2:     m.RotateZ(fDeg); break;
    }

    if ( bLocal )   m_LocalMatrix = m * m_LocalMatrix;
    else            m_LocalMatrix = m_LocalMatrix * m;

    m_bNeedUpdate = true;
}

void RtgNode::Rotate(float fDeg, float x, float y, float z, int bLocal)
{
    if ( bLocal )
    {
        m_LocalMatrix.Rotate(fDeg, x, y, z);
    }else
    {
        RtgMatrix16 m;
        m.Identity();
        m.Rotate(fDeg, x, y, z);
        m_LocalMatrix = m_LocalMatrix * m;
    }

    m_bNeedUpdate = true;
}

void RtgNode::Quaternion(float w, float x, float y, float z, int bLocal)
{
    RtgVertex4 v(x,y,z,w);
    if ( bLocal )
    {
        m_LocalMatrix.MatrixFromQuaternion(v);
    }else
    {
        RtgMatrix16 m;
        m.Identity();
        m.MatrixFromQuaternion(v);
        m_LocalMatrix = m_LocalMatrix * m;
    }
    m_bNeedUpdate = true;
}

void RtgNode::Clone(RtgNode *pNode)
{
    if ( pNode==NULL )
        return;

    pNode->m_LocalMatrix = m_LocalMatrix;
    pNode->m_Matrix = m_Matrix;
    pNode->m_pParent = m_pParent;
    pNode->m_bAddToScene = m_bAddToScene;
    pNode->m_bDelByParent = m_bDelByParent;

    if ( m_pParent )
    {
        pNode->SetParent(m_pParent);
    }
}

void RtgNode::AddChildren(RtgNode *pNode, bool bAutoDel)
{
    if ( pNode==NULL )
        return;
    if ( pNode==this )
        return;
    m_iChildrenCount ++;
    pNode->m_pPrevBrother = m_pLastChildren;
    pNode->m_pNextBrother = NULL;
    pNode->m_pParent = this;
    if (m_pLastChildren)
    {
        m_pLastChildren->m_pNextBrother = pNode;
    }else
    {
        m_pLastChildren = pNode;
        m_pFirstChildren = pNode;
    }
    pNode->m_bNeedUpdate = true;
    pNode->m_bDelByParent = bAutoDel;
}

void RtgNode::DeleteChildren(RtgNode *pNode)
{
    if ( pNode==NULL )
        return;
    if ( pNode==this )
        return;
    if (pNode->m_pParent!=this)
        return;
    m_iChildrenCount --;
    if (pNode->m_pNextBrother) pNode->m_pNextBrother->m_pPrevBrother = pNode->m_pPrevBrother;
    else                       m_pLastChildren = pNode->m_pPrevBrother;
    if (pNode->m_pPrevBrother) pNode->m_pPrevBrother->m_pNextBrother = pNode->m_pNextBrother;
    else                       m_pFirstChildren = pNode->m_pNextBrother;
    pNode->m_pNextBrother = NULL;
    pNode->m_pPrevBrother = NULL;
    pNode->m_pParent = NULL;
}

void RtgNode::SetParent(RtgNode *pNode, bool bAutoDel)
{
    if ( pNode==NULL )
        return;
    if ( m_pParent==pNode )
        return;
    if (m_pParent)
    {
        m_pParent->DeleteChildren(this);
    }
    if ( pNode )
    {
        pNode->AddChildren(this, bAutoDel);
    }
}

void RtgNode::UpdateChilrenMatrix(void)
{
    UpdateNodeMatrix();
    RtgNode* pNode;
    for (pNode=m_pFirstChildren; pNode; pNode=pNode->m_pNextBrother)
    {
        pNode->UpdateChilrenMatrix();
    }
}

void RtgNode::Output()
{
}

} // namespace rt_graph
/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/

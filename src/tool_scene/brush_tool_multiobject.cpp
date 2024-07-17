#include "scene/rt_scene.h"
#include "rts_scene_tool.h"
#include "brush_tool.h"
#include "brush_tool_multobject.h"

/*----------------------------------------------------------------------
    CBrusherMultObject
----------------------------------------------------------------------*/

CBrusherMultObject::CBrusherMultObject(CBrushTool* pBrushTool) : CBrusher(pBrushTool)
{
}

CBrusherMultObject::~CBrusherMultObject()
{
}

void CBrusherMultObject::OnActive()
{
}

BOOL CBrusherMultObject::OnBeginBrush(int iMouseBtn, int iKey, int iMouseX, int iMouseY)
{
//    guard;
    if (iMouseBtn==1)   // 只有按下鼠标左键的时候才能开始刷
        return TRUE;
    return FALSE;
  //  unguard;
}

/*
int cbTileTouch(int iLevel, int x, int y, float fDist, long p1, long p2)
{
    //LOG4("Lev%d, %d, %d, %f\n", iLevel, x, y, fDist);
    //((CBrusherMultObject*)p1)->OnBrushTouch(p2, iLevel, fDist, x, y);
    return 0;
}
*/

BOOL CBrusherMultObject::OnBrushMove(int iKey, int iMouseX, int iMouseY)
{
    //LOG2("--------- %d, %d\n", iTileX, iTileY);
    //m_pBrushTool->CallbackTouch(cbTileTouch, (long)this, iKey);
    return TRUE;
}

void CBrusherMultObject::OnBrushTouch(int iKey, int iLevel, float fDist, int iTileX, int iTileY)
{
}

void CBrusherMultObject::OnEndBrush(int iKey, int iMouseX, int iMouseY)
{
}

/*----------------------------------------------------------------------------
    The End.
----------------------------------------------------------------------------*/

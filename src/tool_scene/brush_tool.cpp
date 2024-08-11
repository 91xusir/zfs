#include "stdafx.h"
#include "scene/rt_scene.h"
#include "rts_scene_tool.h"
#include "brush_tool.h"
#include "brush_tool_vertex.h"
#include "brush_tool_tile.h"
#include "brush_tool_object.h"
#include "brush_tool_water.h"
#include "brush_tool_grass.h"
#include "brush_tool_single_vertex.h"

#include "SceneEditorDoc.h"
#include "SceneEditorView.h"
#include "EditorFrame.h"
//jiang add
#include"DlgNpc.h"

CDrawerBrush::CDrawerBrush()
{
    //guard;
    int i, j;
    float fCurDegree=0.f;
    float fStepDegree = PIx2/DB_MAX_ROUND_QUALITY;
    for (i=0; i<DB_MAX_ROUND_QUALITY; i++)
    {
        m_VertexBuffer[i*2].x = fsin(fCurDegree);
        m_VertexBuffer[i*2].y = fcos(fCurDegree);
        fCurDegree += fStepDegree;
    }
    for (i=0; i<DB_MAX_ROUND_QUALITY-1; i++)
    {
        m_VertexBuffer[i*2+1].x = m_VertexBuffer[i*2+2].x;
        m_VertexBuffer[i*2+1].y = m_VertexBuffer[i*2+2].y;
    }
    m_VertexBuffer[DB_MAX_ROUND_QUALITY*2-1].x = m_VertexBuffer[0].x;
    m_VertexBuffer[DB_MAX_ROUND_QUALITY*2-1].y = m_VertexBuffer[0].y;

    for (j=0; j<(DB_MAX_SIZE*2+1); j++)
    {
        for (i=0; i<(DB_MAX_SIZE*2+1); i++)
        {
            m_fMask[i][j] = 0.f;
        }
    }

    m_iSize1 = 1;
    m_iSize2Min = m_iSize2Max = 1;
    m_bShow = true;
    SetType(DB_ROUND, FALSE);
    SetSizeType(2);
    //unguard;
}

CDrawerBrush::~CDrawerBrush()
{
}

void CDrawerBrush::SetType(EBrushType eType, bool bDoubleDeck)
{
    //guard;
    m_eType = eType;
    SetDoubleDeck(bDoubleDeck);
    SetSize(m_iSizeMax, m_iSizeMin);
    //unguard;
}

float GetSinRate(float fMin, float fMax, float fLen)
{
    if (fLen>fMax) return 0.f;
    if (fLen<fMin) return 1.f;
    if (fMax<=fMin) return 1.f;
    return (fsin((((fMax-fMin)-(fLen-fMin))/(fMax-fMin)-0.5f)*PI)+1.f)*0.5f;
}

void CDrawerBrush::SetSize(int iMax, int iMin)
{
    //guard;
    int i, j;
    float iLen;
    m_iSizeMin = iMin>DB_MAX_SIZE ? DB_MAX_SIZE : iMin;
    m_iSizeMax = iMax>DB_MAX_SIZE ? DB_MAX_SIZE : iMax;

    if (m_eType==DB_RECTANGLE)
    {
        for (j=0; j<(DB_MAX_SIZE*2+1); j++)
        {
            for (i=0; i<(DB_MAX_SIZE*2+1); i++)
            {
                m_fMask[i][j] = 0.f;
            }
        }
        for (j=-m_iSizeMax; j<=m_iSizeMax+1; j++)
        {
            for (i=-m_iSizeMax; i<=m_iSizeMax+1; i++)
            {
                m_fMask[j+DB_MAX_SIZE][i+DB_MAX_SIZE] = GetSinRate(m_iSizeMin, m_iSizeMax, ((abs(j)>abs(i))?abs(j):abs(i)));
            }
        }
    }else
    {
        for (j=0; j<(DB_MAX_SIZE*2+1); j++)
        {
            for (i=0; i<(DB_MAX_SIZE*2+1); i++)
            {
                iLen = sqrt((float)((i-DB_MAX_SIZE)*(i-DB_MAX_SIZE)+(j-DB_MAX_SIZE)*(j-DB_MAX_SIZE)));
                m_fMask[i][j] = GetSinRate(m_iSizeMin, m_iSizeMax, iLen);
            }
        }

        /*
        // 查找边缘
        for (j=0; j<(DB_MAX_SIZE*2+1); j++)
        {
            for (i=0; i<(DB_MAX_SIZE*2+1); i++)
            {
                if (m_cMask[0][i][j]!=0) continue;
                else if (i>0 && m_cMask[0][i-1][j]) m_cMask[1][i][j] = 1;
                else if (j>0 && m_cMask[0][i][j-1]) m_cMask[1][i][j] = 1;
                else if (i<DB_MAX_SIZE*2 && m_cMask[0][i][j+1]) m_cMask[1][i][j] = 1;
                else if (j<DB_MAX_SIZE*2 && m_cMask[0][i][j+1]) m_cMask[1][i][j] = 1;
            }
        }
        */
    }
    //unguard;
}

void CDrawerBrush::SetSizeType(int iType)
{
    m_iSizeType = iType;
    if (m_iSizeType==1)
    {
        SetSize(m_iSize1, m_iSize1);
    }else
    {
        SetSize(m_iSize2Max, m_iSize2Min);
    }
}

void CDrawerBrush::SetSize1(int iSize)
{
    m_iSize1 = iSize;
    if (m_iSizeType==1)
    {
        SetSize(m_iSize1, m_iSize1);
    }
}

void CDrawerBrush::SetSize2(int iMax, int iMin)
{
    m_iSize2Max = iMax;
    m_iSize2Min = iMin;
    if (m_iSizeType==2)
    {
        SetSize(m_iSize2Max, m_iSize2Min);
    }
}

void CDrawerBrush::SetDoubleDeck(bool bDoubleDeck)
{
    m_bDoubleDeck = bDoubleDeck;
}

void CDrawerBrush::Draw(RtgDevice* pDevice, RtSceneTool& vScene, RtgVertex3& vPos, int iGX, int iGY)
{
    //guard;
    if (m_bShow==false) return;
    RtgVertex3 vBrushPos;

    DWORD dwLineColor = 0xFFFF0000;
    float fHeightOffset = 1.f;
    int i, j, x, y, iSizeMin, iSizeMax;
    float fDist;
    bool bEditTerrain = m_pBrushTool->m_pBrusher->IsEditTerrain() && m_pBrushTool->m_pBrusher->m_eAdsorbType==CBrusher::AT_TERRAIN;
    DWORD dwType = bEditTerrain ? PICK_TERRAIN : PICK_VTERRAIN;
    float fWidth = bEditTerrain ? g_fSceneTerrainTileWidth : g_fSceneTileWidth;

    if (bEditTerrain)
    {
        iGX = (int)((vPos.x+g_pScene->GetCenterX()* g_fSceneBlockWidth)/fWidth) - g_pScene->GetCenterX()*g_iSceneTerrainCntX;
        iGY = (int)((vPos.y+g_pScene->GetCenterY()*g_fSceneBlockHeight)/fWidth) - g_pScene->GetCenterY()*g_iSceneTerrainCntY;
    }

    vBrushPos.x = (iGX+0.5f)*fWidth;
    vBrushPos.y = (iGY+0.5f)*fWidth;
    vBrushPos.z = 0.f;

    pDevice->SetVertexFormat(RTG_VC);

    if (m_eType==DB_RECTANGLE)
    {
        iSizeMax = 0;
        j = (m_iSizeMax-1)*2 + iSizeMax*2;
        for (i=1-m_iSizeMax; i<m_iSizeMax; i++)
        {
            m_LineBuffer[0][j+i*2].v.x   = vBrushPos.x - fWidth/2.f + (fWidth * i);
            m_LineBuffer[0][j+i*2+1].v.x = vBrushPos.x + fWidth/2.f + (fWidth * i);
            m_LineBuffer[0][j+i*2].v.y   = m_LineBuffer[0][j+i*2+1].v.y = vBrushPos.y + (fWidth*m_iSizeMax - fWidth/2.f);
            m_LineBuffer[0][j+i*2].v.z   = m_LineBuffer[0][j+i*2+1].v.z = 255.f * g_fSceneHeightStep;
            m_LineBuffer[0][j+i*2].c     = m_LineBuffer[0][j+i*2+1].c = dwLineColor;
        }
        iSizeMax += ((m_iSizeMax-1)*2+1);
        j = (m_iSizeMax-1)*2 + iSizeMax*2;
        for (i=1-m_iSizeMax; i<m_iSizeMax; i++)
        {
            m_LineBuffer[0][j+i*2].v.x   = vBrushPos.x - fWidth/2.f + (fWidth * i);
            m_LineBuffer[0][j+i*2+1].v.x = vBrushPos.x + fWidth/2.f + (fWidth * i);
            m_LineBuffer[0][j+i*2].v.y   = m_LineBuffer[0][j+i*2+1].v.y = vBrushPos.y - (fWidth*m_iSizeMax - fWidth/2.f);
            m_LineBuffer[0][j+i*2].v.z   = m_LineBuffer[0][j+i*2+1].v.z = 255.f * g_fSceneHeightStep;
            m_LineBuffer[0][j+i*2].c     = m_LineBuffer[0][j+i*2+1].c = dwLineColor;
        }
        iSizeMax += ((m_iSizeMax-1)*2+1);
        j = (m_iSizeMax-1)*2 + iSizeMax*2;
        for (i=1-m_iSizeMax; i<m_iSizeMax; i++)
        {
            m_LineBuffer[0][j+i*2].v.y   = vBrushPos.y - fWidth/2.f + (fWidth * i);
            m_LineBuffer[0][j+i*2+1].v.y = vBrushPos.y + fWidth/2.f + (fWidth * i);
            m_LineBuffer[0][j+i*2].v.x   = m_LineBuffer[0][j+i*2+1].v.x = vBrushPos.x + (fWidth*m_iSizeMax - fWidth/2.f);
            m_LineBuffer[0][j+i*2].v.z   = m_LineBuffer[0][j+i*2+1].v.z = 255.f * g_fSceneHeightStep;
            m_LineBuffer[0][j+i*2].c     = m_LineBuffer[0][j+i*2+1].c = dwLineColor;
        }
        iSizeMax += ((m_iSizeMax-1)*2+1);
        j = (m_iSizeMax-1)*2 + iSizeMax*2;
        for (i=1-m_iSizeMax; i<m_iSizeMax; i++)
        {
            m_LineBuffer[0][j+i*2].v.y   = vBrushPos.y - fWidth/2.f + (fWidth * i);
            m_LineBuffer[0][j+i*2+1].v.y = vBrushPos.y + fWidth/2.f + (fWidth * i);
            m_LineBuffer[0][j+i*2].v.x   = m_LineBuffer[0][j+i*2+1].v.x = vBrushPos.x - (fWidth*m_iSizeMax - fWidth/2.f);
            m_LineBuffer[0][j+i*2].v.z   = m_LineBuffer[0][j+i*2+1].v.z = 255.f * g_fSceneHeightStep;
            m_LineBuffer[0][j+i*2].c     = m_LineBuffer[0][j+i*2+1].c = dwLineColor;
        }
        iSizeMax += ((m_iSizeMax-1)*2+1);
        for (i=0; i<iSizeMax*2; i++)
        {
            if (vScene.GetHeight(m_LineBuffer[0][i].v, fDist, x, y, dwType))
            {
                m_LineBuffer[0][i].v.z -= (fDist-fHeightOffset);
            }else
            {
                m_LineBuffer[0][i].v.z = 0.f;
            }
        }
        if (m_bDoubleDeck)
        {
            iSizeMin = 0;
            j = (m_iSizeMin-1)*2 + iSizeMin*2;
            for (i=1-m_iSizeMin; i<m_iSizeMin; i++)
            {
                m_LineBuffer[1][j+i*2].v.x   = vBrushPos.x - fWidth/2.f + (fWidth * i);
                m_LineBuffer[1][j+i*2+1].v.x = vBrushPos.x + fWidth/2.f + (fWidth * i);
                m_LineBuffer[1][j+i*2].v.y   = m_LineBuffer[1][j+i*2+1].v.y = vBrushPos.y + (fWidth*m_iSizeMin - fWidth/2.f);
                m_LineBuffer[1][j+i*2].v.z   = m_LineBuffer[1][j+i*2+1].v.z = 255.f * g_fSceneHeightStep;
                m_LineBuffer[1][j+i*2].c     = m_LineBuffer[1][j+i*2+1].c = dwLineColor;
            }
            iSizeMin += ((m_iSizeMin-1)*2+1);
            j = (m_iSizeMin-1)*2 + iSizeMin*2;
            for (i=1-m_iSizeMin; i<m_iSizeMin; i++)
            {
                m_LineBuffer[1][j+i*2].v.x   = vBrushPos.x - fWidth/2.f + (fWidth * i);
                m_LineBuffer[1][j+i*2+1].v.x = vBrushPos.x + fWidth/2.f + (fWidth * i);
                m_LineBuffer[1][j+i*2].v.y   = m_LineBuffer[1][j+i*2+1].v.y = vBrushPos.y - (fWidth*m_iSizeMin - fWidth/2.f);
                m_LineBuffer[1][j+i*2].v.z   = m_LineBuffer[1][j+i*2+1].v.z = 255.f * g_fSceneHeightStep;
                m_LineBuffer[1][j+i*2].c     = m_LineBuffer[1][j+i*2+1].c = dwLineColor;
            }
            iSizeMin += ((m_iSizeMin-1)*2+1);
            j = (m_iSizeMin-1)*2 + iSizeMin*2;
            for (i=1-m_iSizeMin; i<m_iSizeMin; i++)
            {
                m_LineBuffer[1][j+i*2].v.y   = vBrushPos.y - fWidth/2.f + (fWidth * i);
                m_LineBuffer[1][j+i*2+1].v.y = vBrushPos.y + fWidth/2.f + (fWidth * i);
                m_LineBuffer[1][j+i*2].v.x   = m_LineBuffer[1][j+i*2+1].v.x = vBrushPos.x + (fWidth*m_iSizeMin - fWidth/2.f);
                m_LineBuffer[1][j+i*2].v.z   = m_LineBuffer[1][j+i*2+1].v.z = 255.f * g_fSceneHeightStep;
                m_LineBuffer[1][j+i*2].c     = m_LineBuffer[1][j+i*2+1].c = dwLineColor;
            }
            iSizeMin += ((m_iSizeMin-1)*2+1);
            j = (m_iSizeMin-1)*2 + iSizeMin*2;
            for (i=1-m_iSizeMin; i<m_iSizeMin; i++)
            {
                m_LineBuffer[1][j+i*2].v.y   = vBrushPos.y - fWidth/2.f + (fWidth * i);
                m_LineBuffer[1][j+i*2+1].v.y = vBrushPos.y + fWidth/2.f + (fWidth * i);
                m_LineBuffer[1][j+i*2].v.x   = m_LineBuffer[1][j+i*2+1].v.x = vBrushPos.x - (fWidth*m_iSizeMin - fWidth/2.f);
                m_LineBuffer[1][j+i*2].v.z   = m_LineBuffer[1][j+i*2+1].v.z = 255.f * g_fSceneHeightStep;
                m_LineBuffer[1][j+i*2].c     = m_LineBuffer[1][j+i*2+1].c = dwLineColor;
            }
            iSizeMin += ((m_iSizeMin-1)*2+1);
            for (i=0; i<iSizeMin*2; i++)
            {
                if (vScene.GetHeight(m_LineBuffer[1][i].v, fDist, x, y, dwType))
                {
                    m_LineBuffer[1][i].v.z -= (fDist-fHeightOffset);
                }else
                {
                    m_LineBuffer[1][i].v.z = 0.f;
                }
            }
        }
    }else if (m_eType==DB_ROUND)
    {
        iSizeMax = DB_MAX_ROUND_QUALITY;

        for (i=0; i<DB_MAX_ROUND_QUALITY*2; i++)
        {
            m_LineBuffer[0][i].v.x = vBrushPos.x + m_VertexBuffer[i].x * m_iSizeMax * g_fSceneTileWidth;
            m_LineBuffer[0][i].v.y = vBrushPos.y + m_VertexBuffer[i].y * m_iSizeMax * g_fSceneTileWidth;
            m_LineBuffer[0][i].v.z = 255.f * g_fSceneHeightStep;
            m_LineBuffer[0][i].c = dwLineColor;
        }
        for (i=0; i<DB_MAX_ROUND_QUALITY; i++)
        {
            if (vScene.GetHeight(m_LineBuffer[0][i*2].v, fDist, x, y, dwType))
            {
                m_LineBuffer[0][i*2].v.z -= (fDist-fHeightOffset);
            }else
            {
                m_LineBuffer[0][i*2].v.z = 0.f;
            }
        }
        for (i=0; i<DB_MAX_ROUND_QUALITY-1; i++)
        {
            m_LineBuffer[0][i*2+1].v.z = m_LineBuffer[0][i*2+2].v.z;
        }
        m_LineBuffer[0][DB_MAX_ROUND_QUALITY*2-1].v.z = m_LineBuffer[0][0].v.z;

        if (m_bDoubleDeck)
        {
            iSizeMin = DB_MAX_ROUND_QUALITY;

            for (i=0; i<DB_MAX_ROUND_QUALITY*2; i++)
            {
                m_LineBuffer[1][i].v.x = vBrushPos.x + m_VertexBuffer[i].x * m_iSizeMin * g_fSceneTileWidth;
                m_LineBuffer[1][i].v.y = vBrushPos.y + m_VertexBuffer[i].y * m_iSizeMin * g_fSceneTileWidth;
                m_LineBuffer[1][i].v.z = 0.f;
                m_LineBuffer[1][i].c = dwLineColor;
            }
            for (i=0; i<DB_MAX_ROUND_QUALITY; i++)
            {
                if (vScene.GetHeight(m_LineBuffer[1][i*2].v, fDist, x, y, dwType))
                {
                    m_LineBuffer[1][i*2].v.z -= (fDist-fHeightOffset);
                }
            }
            for (i=0; i<DB_MAX_ROUND_QUALITY-1; i++)
            {
                m_LineBuffer[1][i*2+1].v.z = m_LineBuffer[1][i*2+2].v.z;
            }
            m_LineBuffer[1][DB_MAX_ROUND_QUALITY*2-1].v.z = m_LineBuffer[1][0].v.z;
        }
    }else if (m_eType==DB_BOX)
    {
        float fHalfSize = 3.f;
        RtgVertex3 vMin, vMax;
        vMax = vMin = vPos;
        vMin.x -= fHalfSize;
        vMin.y -= fHalfSize;
        vMin.z -= fHalfSize;
        vMax.x += fHalfSize;
        vMax.y += fHalfSize;
        vMax.z += fHalfSize;
        pDevice->DrawBox(vMin, vMax, &dwLineColor);
        return;
    }

    pDevice->DrawPrimitiveUP(D3DPT_LINELIST, &(m_LineBuffer[0][0].v), sizeof(RtgVertexVC), iSizeMax);
    if (m_bDoubleDeck)
    {
        pDevice->DrawPrimitiveUP(D3DPT_LINELIST, &(m_LineBuffer[0][0].v), sizeof(RtgVertexVC), iSizeMin);
    }
    //unguard;
}

/*----------------------------------------------------------------------
    CBrusher
----------------------------------------------------------------------*/

void CBrusher::DrawLineBrush(RtgDevice* pDevice, RtSceneTool& vScene, RtgVertex3& vPos, int iGX, int iGY)
{
    funguard;
    this->m_pBrushTool->m_Drawer.Draw(pDevice, vScene, vPos, iGX, iGY);
    fununguard;
}

//jiang add

BOOL CBrusher::OnBeginBrush(int iMouseBtn, int iKey, int iMouseX, int iMouseY)
{
   if (iMouseBtn==1)   // 只有按下鼠标左键的时候才能弹出对话框
   {
     float fDist;
     RtgVertex3 vOrig, vDir;
     int iTileX, iTileY;
	 float mx,my;
	 char szNum[20];
     CEditorFrame* pEditor = (CEditorFrame*)g_pAppCase;
	 RtgMatrix16 m16;
    // if (pEditor->m_bAddNpc)
    // {
	    RtGetRender()->GetPickRayFromScreen(iMouseX, iMouseY, vOrig, vDir);
#ifdef DO_CONSOLE
	    g_pScene->BeginPickRayRecord(vOrig, vDir);
#endif
	    m_pBrushObject = g_pScene->PickRay(vOrig, vDir, fDist, iTileX, iTileY, PICK_OBJECT|PICK_HELPER);
#ifdef DO_CONSOLE
	    g_pScene->EndPickRayRecord();
#endif
		bool bIsNpc = false;
		std::list<BrushNpcInfo>::iterator iterobj;

        //while()
		while(m_pBrushObject)
		{
		
		  for( iterobj = pEditor->m_NpcBrushList.begin();iterobj != pEditor->m_NpcBrushList.end();iterobj++)
		  {
		    if((*iterobj).m_NpcModel == m_pBrushObject)
		    {
             bIsNpc = true;
			 break;
		    }
           }
		   if(bIsNpc)
			 break;
           m_pBrushObject = m_pBrushObject->m_pObjectLink;

		}
	   
	   if (bIsNpc)
	   {
       
	     CDlgNpc dlg((CWnd*)(pEditor->m_pMainFrame)); //点击地形上的NPC会出现NPC对话框
		 dlg.m_SerialNum = (char*)((*iterobj).iSerialNum.c_str());
		 dlg.m_NpcName = (char*)((*iterobj).szNpcName.c_str());
		 dlg.m_NpcFuncName = (char*)((*iterobj).szNpcFuncName.c_str());
		 dlg.m_szModelId = (char*)((*iterobj).iModelId.c_str());

		 mx = *((*iterobj).m_NpcModel->GetMatrix().Position());
		 my = *((*iterobj).m_NpcModel->GetMatrix().Position() + 1);

         sprintf(szNum,"%.2f",mx);
		 dlg.m_szPosx = szNum;
		 sprintf(szNum,"%.2f",my);
		 dlg.m_Posy = szNum;

		 sprintf(szNum,"%.2f",(*iterobj).m_NpcDir.x);
		 dlg.m_Dirx = szNum;
		 sprintf(szNum,"%.2f",(*iterobj).m_NpcDir.y);
         dlg.m_Diry = szNum;

		 dlg.m_szScript = (char*)((*iterobj).szScript.c_str());
		 dlg.m_szDialog = (char*)((*iterobj).szDialog.c_str());
		 dlg.m_szAtten =  (char*)((*iterobj).szAttention.c_str());
		 dlg.m_szVertion = (char*)((*iterobj).szVertion.c_str());
           
		   if (dlg.DoModal()==IDOK)
	       {

			(*iterobj).iSerialNum = dlg.m_SerialNum;
			(*iterobj).szNpcName = dlg.m_NpcName;
			(*iterobj).szNpcFuncName = dlg.m_NpcFuncName;
            (*iterobj).iModelId =  dlg.m_szModelId;
			
			(*iterobj).m_NpcDir.x = atof(dlg.m_Dirx);
            (*iterobj).m_NpcDir.y = atof(dlg.m_Diry);
			//AlterString(dlg.m_szScript);
            (*iterobj).szScript = dlg.m_szScript;
           // AlterString(dlg.m_szDialog);
		    (*iterobj).szDialog = dlg.m_szDialog;
            //AlterString(dlg.m_szAtten);
		    (*iterobj).szAttention = dlg.m_szAtten;
			//AlterString(dlg.m_szVertion);
			(*iterobj).szVertion = dlg.m_szVertion;
			
			 //更新物件的位置和朝向
             g_pScene->RemoveObject(m_pBrushObject->Id());			
             mx = atof(dlg.m_szPosx);
			 my = atof(dlg.m_Posy);
			 m16 = (*iterobj).m_NpcModel->GetMatrix();
             m16._30 = mx;
			 m16._31 = my;
             m16.FaceToVertex((*iterobj).m_NpcDir);
			(*iterobj).m_NpcModel->SetMatrix(m16);
			 g_pScene->AddObject(m_pBrushObject->Id());
		
			
			
			
           }
       }
	// }
   }

  return false;
}

/*
std::string  CBrusher::AlterString(CString p)
{
  int iLen = p.GetLength();
  char a;
  std::string ss;
  for(int j=0;j< p.GetLength();j++)
  {
     a=p[j];
     ss+=a;
     if(j!=0 && j%10 == 0)
	      ss+="\r\n"; 
  }
 
  //p = ss.c_str();
  return ss;
}
*/

/*----------------------------------------------------------------------
    CBrushTool
----------------------------------------------------------------------*/

CBrushTool::CBrushTool()
{
    funguard;
    m_Drawer.m_pBrushTool = this;
    m_bHitTest = FALSE;
    m_iLastSelX = m_iLastSelY = -1;
    m_iLastMoveTerrainX = m_iLastMoveTerrainY = -1;
    m_vLastCursorPos.Set(0.f);
    m_pBrusher = NULL;
    memset(m_pBrusherList, 0, sizeof(CBrusher*)*BT_BRUSH_MAX);

    m_pBrusherList[BT_NONE]         = new CBrusher(this);
    m_pBrusherList[BT_VERTEX]       = new CBrusherVertex(this);
    m_pBrusherList[BT_POING]        = new CBrusherSingleVertex(this);
    m_pBrusherList[BT_TILE_VERTEX]  = new CBrusherTileVertex(this);
    m_pBrusherList[BT_TILE]         = new CBrusherTile(this);
    m_pBrusherList[BT_OBJECT]       = new CBrusherObject(this, CBrusherObject::OOT_APPEND);
    m_pBrusherList[BT_WATER]        = new CBrusherWater(this);
    m_pBrusherList[BT_GRASS]        = new CBrusherGrass(this);

    m_pBrusherList[BT_ATTR_VERTEX]  = new CBrusherAttribute(this, 0x00);
    m_pBrusherList[BT_ATTR_WALK]    = new CBrusherAttribute(this, 0x01);
    m_pBrusherList[BT_ATTR_FLY]     = new CBrusherAttribute(this, 0x02);
    m_pBrusherList[BT_ATTR_DIG]     = new CBrusherAttribute(this, 0x04);

    m_pBrusherList[BT_OBJECT_MOVE]      = new CBrusherObject(this, CBrusherObject::OOT_MOVE);
    m_pBrusherList[BT_OBJECT_ADJUST]    = new CBrusherObject(this, CBrusherObject::OOT_ADJUST);

    SetBrushType(BT_NONE);
    fununguard;
}

CBrushTool::~CBrushTool()
{
    funguard;
    for (int i=0; i<BT_BRUSH_MAX; i++)
    {
       DEL_ONE(m_pBrusherList[BT_NONE]);
    }
    fununguard;
}

void CBrushTool::SetBrushType(EBrushType eType)
{
	funguard;
    if (m_pBrusher) m_pBrusher->OnExit();
    m_eBrushType = eType;
    m_pBrusher = m_pBrusherList[m_eBrushType];
    if (m_pBrusher==NULL) SetBrushType(BT_NONE);
    if (m_eBrushType==BT_TILE)
    {
        m_Drawer.SetSizeType(1);
    }else
    {
        m_Drawer.SetSizeType(2);
    }
    m_pBrusher->OnActive();
    //unguard;
	fununguard;
}

void CBrushTool::OnBeginRender(RtgDevice* pDevice, RtSceneTool& vScene)
{
    funguard;
    if (m_pBrusher!=NULL && m_bHitTest)
    {
        m_pBrusher->OnBeginRender(pDevice, vScene, m_vLastCursorPos, m_iLastSelX, m_iLastSelY);
    }
    fununguard;
}

void CBrushTool::OnEndRender(RtgDevice* pDevice, RtSceneTool& vScene)
{
    funguard;
    if (m_pBrusher!=NULL && m_bHitTest)
    {
        m_pBrusher->OnEndRender(pDevice, vScene, m_vLastCursorPos, m_iLastSelX, m_iLastSelY);
    }
    fununguard;
}

void CBrushTool::OnRender(RtgDevice* pDevice, RtSceneTool& vScene)
{
    funguard;
    if (m_pBrusher!=NULL && m_bHitTest )
    {
        m_pBrusher->OnRender(pDevice, vScene, m_vLastCursorPos, m_iLastSelX, m_iLastSelY);
    }
    fununguard;
}

BOOL CBrushTool::OnBeginBrush(int iMouseBtn, int iKey, int iMouseX, int iMouseY)
{
    funguard;
    return m_pBrusher->OnBeginBrush(iMouseBtn, iKey, iMouseX, iMouseY);
    fununguard;
}

void CBrushTool::OnBrushMove(int iKey, int iMouseX, int iMouseY)
{
    funguard;
    m_pBrusher->OnBrushMove(iKey, iMouseX, iMouseY);
    fununguard;
}

void CBrushTool::OnEndBrush(int iKey, int iMouseX, int iMouseY)
{
    funguard;
    m_pBrusher->OnEndBrush(iKey, iMouseX, iMouseY);
    fununguard;
}

void CBrushTool::SetBrushData(long lParam1, long lParam2, long lParam3)
{
    funguard;
    m_pBrusher->SetBrushData(lParam1, lParam2, lParam3);
    fununguard;
}

void CBrushTool::Command(int iCmdID, long lParam1, long lParam2)
{
    funguard;
    m_pBrusher->OnCommand(iCmdID, lParam1, lParam2);
    fununguard;
}

void CBrushTool::OnFrameMove(float fDiffTime)
{
    if (g_pScene==NULL) return;
    RtgVertex3 vOrig, vDir;
    int iOutX, iOutY;
    RtsObject* pObj;
    DWORD dwType;
    float fDist;
    POINT pt;
    bool bEditTerrain = m_pBrusher->IsEditTerrain() && m_pBrusher->m_eAdsorbType==CBrusher::AT_TERRAIN;

    if (bEditTerrain || (m_pBrusher->m_eAdsorbType==CBrusher::AT_NONE && (GetKeyState(VK_CONTROL)&0xFF00)))
    {
        dwType = PICK_TERRAIN;
    }else
    {
        dwType = PICK_VTERRAIN;
    }
    GetCursorPos(&pt);
    ScreenToClient((HWND)(g_pAppCase->GetWndHandle()), &pt);
    RtGetRender()->GetPickRayFromScreen(pt.x, pt.y, vOrig, vDir);
    pObj = g_pScene->PickRay(vOrig, vDir, fDist, iOutX, iOutY, dwType);
    if (pObj==NULL || fDist<0.f)
    {
        m_vLastCursorPos.Set(0.f);
        m_bHitTest = FALSE;
        m_iLastSelX = m_iLastSelY = -1;
        m_iLastMoveTerrainX = m_iLastMoveTerrainY = -1;
        return;
    }
    m_bHitTest = TRUE;
    m_vLastCursorPos = vOrig + vDir*fDist;
    g_pScene->GetTerrainByPosFast(m_vLastCursorPos.x, m_vLastCursorPos.y, m_iLastMoveTerrainX, m_iLastMoveTerrainY);

    switch (m_pBrusher->m_eAdsorbType)
    {
    case CBrusher::AT_VERTEX:
        g_pScene->OffsetToMap(m_vLastCursorPos.x, m_vLastCursorPos.y);
        if (dwType==PICK_VTERRAIN)
        {
            iOutX = m_vLastCursorPos.x/(g_fSceneTileWidth/2.f);
            iOutY = m_vLastCursorPos.y/(g_fSceneTileWidth/2.f);
            g_pScene->OffsetTerrainToWorld(iOutX, iOutY);
        }
        g_pScene->OffsetTerrainToMap(iOutX, iOutY);
        m_iLastSelX = (iOutX+1)/2;
        m_iLastSelY = (iOutY+1)/2;
        g_pScene->OffsetToWorld(m_iLastSelX, m_iLastSelY);
        m_vLastCursorPos.x = g_fSceneTileWidth * m_iLastSelX;
        m_vLastCursorPos.y = g_fSceneTileWidth * m_iLastSelY;
        m_vLastCursorPos.z = g_pScene->GetVertexHeightLevel(m_iLastSelX, m_iLastSelY, dwType==PICK_VTERRAIN) * g_fSceneHeightStep;
        break;
    case CBrusher::AT_TILE:
        m_vLastCursorPos.x = (((int)m_vLastCursorPos.x)/5)*5;
        m_vLastCursorPos.y = (((int)m_vLastCursorPos.y)/5)*5;
        m_vLastCursorPos.z += g_fSceneHeightStep * 255;
        g_pScene->GetHeight(m_vLastCursorPos, fDist, iOutX, iOutY, dwType);
        m_vLastCursorPos.z = m_vLastCursorPos.z - fDist;
        m_iLastSelX = iOutX;
        m_iLastSelY = iOutY;
        if (dwType==PICK_TERRAIN)
        {
            m_iLastSelX /= 2;
            m_iLastSelY /= 2;
        }
        break;
    case CBrusher::AT_NONE:
    case CBrusher::AT_TERRAIN:
        m_vLastCursorPos.x = (((int)m_vLastCursorPos.x)/5)*5;
        m_vLastCursorPos.y = (((int)m_vLastCursorPos.y)/5)*5;
        m_vLastCursorPos.z += g_fSceneHeightStep * 255;
        g_pScene->GetHeight(m_vLastCursorPos, fDist, iOutX, iOutY, dwType);
        m_vLastCursorPos.z = m_vLastCursorPos.z - fDist;
        m_iLastSelX = iOutX;
        m_iLastSelY = iOutY;
        break;
    }

    m_pBrusher->OnFrameMove(fDiffTime);
}

void CBrushTool::GetTerrainPos(int iMouseX, int iMouseY, int& iOutX, int& iOutY, bool bViewTerrain, bool bGetVertex)
{
    float fDist;
    RtgVertex3 vOrig, vDir;
    DWORD dwType = bViewTerrain ? PICK_VTERRAIN : PICK_TERRAIN;
    RtGetRender()->GetPickRayFromScreen(iMouseX, iMouseY, vOrig, vDir);
    RtsObject* pObj = g_pScene->PickRay(vOrig, vDir, fDist, iOutX, iOutY, dwType);
    if (pObj && fDist>0.f)
    {
        m_iLastTileX = iOutX;
        m_iLastTileY = iOutY;
        if (bGetVertex)
        {
            if (bViewTerrain)
            {
                vOrig = vOrig + vDir*fDist;
                g_pScene->OffsetToMap(vOrig.x, vOrig.y);
                iOutX = vOrig.x/(g_fSceneTileWidth/2.f);
                iOutY = vOrig.y/(g_fSceneTileWidth/2.f);
                g_pScene->OffsetTerrainToWorld(iOutX, iOutY);
            }
            g_pScene->OffsetTerrainToMap(iOutX, iOutY);
            iOutX = (iOutX+1)/2;
            iOutY = (iOutY+1)/2;
            g_pScene->OffsetToWorld(iOutX, iOutY);
        }
    }else
    {
        m_iLastTileX = iOutX;
        m_iLastTileY = iOutY;
        iOutX = -1;
        iOutY = -1;
    }
}

// iLevel 层次, 0 内圈, 1 中层
// x,y 坐标
// fDist 离中心点的百分比, 0在中心点, 1在最外圈，如果是中间层，就是离内圈距离的半分比, 0就正好在内圈上
// p1, p2 传入参数
int CBrushTool::CallbackTouch(int (__cdecl *cbTouch)(int iLevel, int x, int y, float fDist, long p1, long p2), long p1, long p2)
{
    //guard;
    int xi1, yi1, xi2, yi2, o;
    int xa1, ya1, xa2, ya2, x0, y0;
    float fDist, fDistNear, fDistFar;
    if (m_Drawer.m_eType==CDrawerBrush::DB_RECTANGLE)
    {
        xi1 = m_iLastTileX-m_Drawer.m_iSizeMax+1;
        yi1 = m_iLastTileY-m_Drawer.m_iSizeMax+1;
        xi2 = m_iLastTileX+m_Drawer.m_iSizeMax;
        yi2 = m_iLastTileY+m_Drawer.m_iSizeMax;
        if (!m_Drawer.m_bDoubleDeck)
        {
            for (y0=yi1; y0<yi2; y0++)
            {
                for (x0=xi1; x0<xi2; x0++)
                {
                    cbTouch(0, x0, y0, 0.f, p1, p2);
                }
            }
        }
    }else
    {
        fDistNear = m_Drawer.m_iSizeMax;
        o = fDistNear;
        xi1 = m_iLastTileX-o;
        yi1 = m_iLastTileY-o;
        xi2 = m_iLastTileX+o;
        yi2 = m_iLastTileY+o;
        if (xi1<1) xi1 = 1;
        if (xi2>g_iSceneTileCntX-1) xi2 = g_iSceneTileCntX-1;
        if (yi1<1) yi1 = 1;
        if (yi2>g_iSceneTileCntY-1) yi2 = g_iSceneTileCntY-1;
        fDistFar = m_Drawer.m_iSizeMax;
        o = fDistFar;
        xa1 = m_iLastTileX-o;
        ya1 = m_iLastTileY-o;
        xa2 = m_iLastTileX+o;
        ya2 = m_iLastTileY+o;
        if (xa1<1) xa1 = 1;
        if (xa2>g_iSceneTileCntX-2) xa2 = g_iSceneTileCntX-2;
        if (ya1<1) ya1 = 1;
        if (ya2>g_iSceneTileCntY-2) ya2 = g_iSceneTileCntY-2;
        if (m_Drawer.m_bDoubleDeck)
        {
            for (y0=ya1; y0<ya2; y0++)
            {
                for (x0=xa1; x0<xa2; x0++)
                {
                    fDist = fsqrt((float)((x0-m_iLastTileX)*(x0-m_iLastTileX)+(y0-m_iLastTileY)*(y0-m_iLastTileY)));
                    if (fDist<fDistNear)
                    {
                        cbTouch(0, x0, y0, fDist/fDistNear, p1, p2);
                    }else if (fDist<fDistFar)
                    {
                        cbTouch(1, x0, y0, (fDist-fDistNear)/(fDistFar-fDistNear), p1, p2);
                    }
                }
            }
        }else
        {
            for (y0=yi1; y0<yi2; y0++)
            {
                for (x0=xi1; x0<xi2; x0++)
                {
                    fDist = fsqrt((float)((x0-m_iLastTileX)*(x0-m_iLastTileX)+(y0-m_iLastTileY)*(y0-m_iLastTileY)));
                    if (fDist<fDistNear)
                    {
                        cbTouch(0, x0, y0, fDist/fDistNear, p1, p2);
                    }
                }
            }
        }
    }
    return 0;
    //unguard;
}

int CBrushTool::CallbackSmallTouch(int (__cdecl *cbTouch)(int x, int y, int iIdx, float fDist, long p1, long p2), long p1, long p2)
{
    //guard;
    int xi1, yi1, xi2, yi2, o, x0, y0;
    float fDist, fDistNear, x1, y1;
    if (m_Drawer.m_eType==CDrawerBrush::DB_RECTANGLE)
    {
        xi1 = m_iLastTileX-m_Drawer.m_iSizeMax+1 + g_pScene->GetCenterX()*g_iSceneTerrainCntX;
        yi1 = m_iLastTileY-m_Drawer.m_iSizeMax+1 + g_pScene->GetCenterY()*g_iSceneTerrainCntY;
        xi2 = m_iLastTileX+m_Drawer.m_iSizeMax   + g_pScene->GetCenterX()*g_iSceneTerrainCntX;
        yi2 = m_iLastTileY+m_Drawer.m_iSizeMax   + g_pScene->GetCenterY()*g_iSceneTerrainCntY;
        if (!m_Drawer.m_bDoubleDeck)
        {
            for (y0=yi1; y0<yi2; y0++)
            {
                for (x0=xi1; x0<xi2; x0++)
                {
                    x1 = x0/2 - g_pScene->GetCenterX()*g_iSceneTileCntX;
                    y1 = y0/2 - g_pScene->GetCenterY()*g_iSceneTileCntY;
                    cbTouch(x1, y1, (y0%2)*2+(x0%2), 0.f, p1, p2);
                }
            }
        }
    }else
    {
        fDistNear = m_Drawer.m_iSizeMax;
        o = fDistNear;
        xi1 = m_iLastTileX-o;
        yi1 = m_iLastTileY-o;
        xi2 = m_iLastTileX+o;
        yi2 = m_iLastTileY+o;
        for (y0=yi1; y0<=yi2; y0++)
        {
            for (x0=xi1; x0<=xi2; x0++)
            {
                x1 = x0; y1 = y0;
                fDist = fsqrt((float)((x1-m_iLastTileX)*(x1-m_iLastTileX)+(y1-m_iLastTileY)*(y1-m_iLastTileY)));
                if (fDist<fDistNear)
                    cbTouch(x0, y0, 0, fDist/fDistNear, p1, p2);

                x1 = x0+0.5f; y1 = y0;
                fDist = fsqrt((float)((x1-m_iLastTileX)*(x1-m_iLastTileX)+(y1-m_iLastTileY)*(y1-m_iLastTileY)));
                if (fDist<fDistNear)
                    cbTouch(x0, y0, 1, fDist/fDistNear, p1, p2);

                x1 = x0; y1 = y0+0.5f;
                fDist = fsqrt((float)((x1-m_iLastTileX)*(x1-m_iLastTileX)+(y1-m_iLastTileY)*(y1-m_iLastTileY)));
                if (fDist<fDistNear)
                    cbTouch(x0, y0, 2, fDist/fDistNear, p1, p2);

                x1 = x0+0.5f; y1 = y0+0.5f;
                fDist = fsqrt((float)((x1-m_iLastTileX)*(x1-m_iLastTileX)+(y1-m_iLastTileY)*(y1-m_iLastTileY)));
                if (fDist<fDistNear)
                    cbTouch(x0, y0, 3, fDist/fDistNear, p1, p2);
            }
        }
    }
    return 0;
    //unguard;
}

void CBrushTool::OriginallyObjectSize()
{
    ((CBrusherObject*)m_pBrusherList[BT_OBJECT])->OriginallyObjectSize();
}

/*----------------------------------------------------------------------------
    The End.
----------------------------------------------------------------------------*/

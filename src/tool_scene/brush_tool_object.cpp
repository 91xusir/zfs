#include "stdafx.h"
#include "tool_scene.h"
#include "scene/rt_scene.h"
#include "rts_scene_tool.h"
#include "brush_tool.h"
#include "brush_tool_object.h"

#include "SceneEditorDoc.h"
#include "SceneEditorView.h"
#include "EditorFrame.h"
#include "resource.h"
#include "string.h"
#include ".\brushvertexview.h"
#include "EditorFrame.h"

RtsObject* g_pMenuSelObject[20];

extern CBrushVertexForm*   gpkTerrainBrush;

/*----------------------------------------------------------------------
    CBrusherObject
----------------------------------------------------------------------*/

CBrusherObject::CBrusherObject(CBrushTool* pBrushTool, EObjOpType eOp) : CBrusher(pBrushTool)
{
//    guard;
    m_eOpType = eOp;
    m_pLastSelectObject = NULL;
    m_pBrushObject = NULL;
    m_pRuntimeClass = NULL;
    m_bActive = false;
    m_lObjectType = 0;
    m_bLightObject = false;
    m_bInPopupMenu = false;
    m_OffsetMatrix.Unit();

    OriginallyObjectSize();
  //  unguard;
}

void CBrusherObject::OriginallyObjectSize()
{
    m_fObjectScale = 1.f;
    m_OffsetLight.Unit();
    m_OffsetLight._32 += 20.f;
    m_OffsetAudio.Unit();
    m_OffsetAudio._32 += 20.f;
    m_OffsetObject.Unit();
}

CBrusherObject::~CBrusherObject()
{
}

void CBrusherObject::SetBrushData(long lParam1, long lParam2, long lParam3)
{
    //guard;
    if (m_eOpType==OOT_APPEND)
    {
		char* szFile = (char*)lParam1;
		char* wav = strrchr(szFile, '.wav');
		if (!wav)
		{
			char* t = strrchr(szFile, '.');
			if (t)
				*t = 0;
		}

		//RtString str = szFile;
		//str.TrimRight(".act");
		//szFile = str;
        long lType = lParam2;
        long* pResult = (long*)lParam3;
        if (CreateObject(lType, szFile))
        {
            *pResult = 1;
        }
    }
    //unguard;
}

void CBrusherObject::OnActive()
{
    m_bActive = true;
}

void CBrusherObject::OnExit()
{
    //guard;
	funguard;
    m_bActive = false;
    if (m_bLightObject)
    {
        g_pScene->m_iCustomLightCount = 0;
        //g_pDevice->SetLight(1, NULL);
    }
    if (m_eOpType==OOT_APPEND)
    {
        DeleteObject();
    }
    m_pBrushObject = NULL;
    m_pLastSelectObject = NULL;
    //unguard;
	fununguard;
}

void CBrusherObject::OnCommand(int iCmdID, long lParam1, long lParam2)
{		
	if( gpkTerrainBrush ) gpkTerrainBrush->SetOperatorObject( NULL );
    switch (iCmdID)
    {
    case 1: // Delete
        if (m_eOpType==OOT_MOVE || m_eOpType==OOT_ADJUST)
        {
            if (m_pLastSelectObject)
            {
                if (m_pBrushObject==NULL)
                {
                    RemoveObjectFromScene(m_pLastSelectObject);
                    RtObjectManager()->DelObject(m_pLastSelectObject->Id());
                    m_pLastSelectObject = NULL;
					
                }else
                {
					MessageBox(NULL,"警告: 移动状态不能删除物件","ERROR",MB_OK);
                }
            }
        }
        break;
    case 2:
        if (m_eOpType==OOT_ADJUST)
        {
            if (m_pLastSelectObject)
            {
                // 从场景中删掉物体
                m_pBrushObject = m_pLastSelectObject;
                RemoveObjectFromScene(m_pBrushObject);
                if (m_pBrushObject->IsKindOf(RT_RUNTIME_CLASS(RtsLight)))
                {
                    m_lObjectType = 1;
                }else if (m_pBrushObject->IsKindOf(RT_RUNTIME_CLASS(RtsAudio3D)))
                {
                    m_lObjectType = 2;
                }else if (m_pBrushObject->IsKindOf(RT_RUNTIME_CLASS(RtsModel)))
                {
                    m_lObjectType = 3;
                }else
                {
                    m_lObjectType = 0;
                }
                // 根据命令进行移动
                RtgMatrix16 m16 = m_pBrushObject->GetMatrix();
                RtgMatrix16 m16Backup = m16;
                float fMoveStep = 1.f;
                float fScale;
				if(GetKeyState(VK_LCONTROL) & 0x1000)
				{
					fMoveStep = 10.0f;
				}
				else
				{
					fMoveStep = 1.0f;
				}
                switch (lParam1)
                {
                case VK_LEFT:       // 向左移动
                    m16._30 -= fMoveStep;
                    m_pBrushObject->SetMatrix(m16);
                    if (m_lObjectType==1) ((RtsLight*)m_pBrushObject)->vPosition.x -= fMoveStep;
                    break;
                case VK_RIGHT:      // 向右移动
                    m16._30 += fMoveStep;
                    m_pBrushObject->SetMatrix(m16);
                    if (m_lObjectType==1) ((RtsLight*)m_pBrushObject)->vPosition.x += fMoveStep;
                    break;
                case VK_UP:         // 向前移动
                    m16._31 += fMoveStep;
                    m_pBrushObject->SetMatrix(m16);
                    if (m_lObjectType==1) ((RtsLight*)m_pBrushObject)->vPosition.y += fMoveStep;
                    break;
                case VK_DOWN:       // 向后移动
                    m16._31 -= fMoveStep;
                    m_pBrushObject->SetMatrix(m16);
                    if (m_lObjectType==1) ((RtsLight*)m_pBrushObject)->vPosition.y -= fMoveStep;
                    break;
                case VK_PRIOR:      // 向上移动
                    m16._32 += fMoveStep;
                    m_pBrushObject->SetMatrix(m16);
                    if (m_lObjectType==1) ((RtsLight*)m_pBrushObject)->vPosition.z += fMoveStep;
                    break;
                case VK_NEXT:       // 向下移动
                    m16._32 -= fMoveStep;
                    m_pBrushObject->SetMatrix(m16);
                    if (m_lObjectType==1) ((RtsLight*)m_pBrushObject)->vPosition.z -= fMoveStep;
                    break;
                case VK_OEM_PLUS:   // 放大
                    if (m_lObjectType==3) // RtsModel
                    {
                        fScale = 1.1f;
                        m16.Scale(fScale, fScale, fScale);
                        m_pBrushObject->SetMatrix(m16);
                      //  ((RtsModel*)m_pBrushObject)->UpdateModelAABB();
                    }
                    break;
                case VK_OEM_MINUS:  // 缩小
                    if (m_lObjectType==3) // RtsModel
                    {
                        fScale = 0.9f;
                        m16.Scale(fScale, fScale, fScale);
                        m_pBrushObject->SetMatrix(m16);
                    //    ((RtsModel*)m_pBrushObject)->UpdateModelAABB();
                    }
                    break;
                case VK_OEM_4:      // 向左旋转
                    if (m_lObjectType==3) // RtsModel
                    {
                        m16.RotateLZ(PI*0.25f);
                        m_pBrushObject->SetMatrix(m16);
                    }
                    break;
                case VK_OEM_6:      // 向右旋转
                    if (m_lObjectType==3) // RtsModel
                    {
                        m16.RotateLZ(-PI*0.25f);
                        m_pBrushObject->SetMatrix(m16);
                    }
                    break;
                }

                if (!AddObjectToScene(0, 0))
                {
                    m_pBrushObject->SetMatrix(m16Backup);

                    if (!AddObjectToScene(0, 0))
                    {
                        g_pScene->RemoveObject(m_pBrushObject->Id());
                        m_pLastSelectObject = NULL;
                    }
                }

                m_pBrushObject = NULL;
				
            }
        }
        break;
    default:
        break;
    }
}

BOOL CBrusherObject::OnBeginBrush(int iMouseBtn, int iKey, int iMouseX, int iMouseY)
{
    //guard;
    if (iMouseBtn==1)   // 只有按下鼠标左键的时候才能开始刷
    {
        float fDist;
        RtgVertex3 vOrig, vDir;
        int iTileX, iTileY;
        if (m_eOpType==OOT_APPEND)
        {
            if (!AddObjectToScene(iMouseX, iMouseY))
            {
                return FALSE;
            }
            switch (m_lObjectType)
            {
            case 1: // light
                m_OffsetLight = m_OffsetMatrix;
                break;
            case 2: // audio
                m_OffsetAudio = m_OffsetMatrix;
                break;
            case 3: // object
                m_OffsetObject = m_OffsetMatrix;
                break;
            }
            // 创建一个新的物件，用于新的一次刷子
            m_pBrushObject = NULL;
            CreateObject(m_lObjectType, m_strObjectFile);
            return TRUE;
        }else if (m_eOpType==OOT_MOVE)
        {   // 选择移动物体
            RtGetRender()->GetPickRayFromScreen(iMouseX, iMouseY, vOrig, vDir);
			
#ifdef DO_CONSOLE
            g_pScene->BeginPickRayRecord(vOrig, vDir);
#endif
            m_pBrushObject = g_pScene->PickRay(vOrig, vDir, fDist, iTileX, iTileY, PICK_OBJECT|PICK_HELPER);
#ifdef DO_CONSOLE
            g_pScene->EndPickRayRecord();
#endif
            if (m_pBrushObject)
            {
                m_pLastSelectObject = m_pBrushObject;
                if (m_pBrushObject->IsKindOf(RT_RUNTIME_CLASS(RtsLight)))
                {
                    m_bLightObject = true;
                    m_lObjectType = 1;
                }else if (m_pBrushObject->IsKindOf(RT_RUNTIME_CLASS(RtsAudio3D)))
                {
                    m_lObjectType = 2;
                }else if (m_pBrushObject->IsKindOf(RT_RUNTIME_CLASS(RtsModel)))
                {
                    m_lObjectType = 3;
                }else
                {
                    m_lObjectType = 0;
                }
                RemoveObjectFromScene(m_pBrushObject);
                fDist = 0.f;
                vOrig = m_pBrushObject->GetMatrix().Position();
                g_pScene->GetHeight(vOrig, fDist, iTileX, iTileY, PICK_VTERRAIN);
                m_OffsetMatrix = m_pBrushObject->GetMatrix();
                m_OffsetMatrix._30 = m_OffsetMatrix._31 = 0.f;
                m_OffsetMatrix._32 = fDist;
            }
            return TRUE;
        }else if (m_eOpType==OOT_ADJUST)
        {
            RtGetRender()->GetPickRayFromScreen(iMouseX, iMouseY, vOrig, vDir);
#ifdef DO_CONSOLE
            g_pScene->BeginPickRayRecord(vOrig, vDir);
#endif
            m_pBrushObject = g_pScene->PickRay(vOrig, vDir, fDist, iTileX, iTileY, PICK_OBJECT|PICK_HELPER);
#ifdef DO_CONSOLE
            g_pScene->EndPickRayRecord();
#endif
			if( gpkTerrainBrush ) gpkTerrainBrush->SetOperatorObject( m_pBrushObject );
            if (m_pBrushObject)
            {				
			//	if( gpkTerrainBrush ) gpkTerrainBrush->SetFadeOutFadeIn( m_pBrushObject->m_bFadeOutFadeIn );
                if (m_pBrushObject->m_pObjectLink)
                {
                    CMenu menu;
                    menu.LoadMenu(IDR_MENU_MULTIOBJ);
                    char szMenuString[120];
                    int iSelCnt = 0;
                    g_pMenuSelObject[0] = NULL;
                    RtsObject* pObj = m_pBrushObject;
                    while (pObj && iSelCnt<19)
                    {
                        g_pMenuSelObject[iSelCnt] = pObj;
                        iSelCnt ++;
                        if (pObj->IsKindOf(RT_RUNTIME_CLASS(RtsModel)))
                        {
                            sprintf(szMenuString, "%2d: %s", iSelCnt, ((RtsModel*)pObj)->m_fileName.c_str());
                        }else if (pObj->IsKindOf(RT_RUNTIME_CLASS(RtsAudio3D)))
                        {
                            sprintf(szMenuString, "%2d: %s", iSelCnt, ((RtsAudio3D*)pObj)->m_fileName.c_str());
                        }else
                        {
                            sprintf(szMenuString, "%2d: %s", iSelCnt, pObj->GetRuntimeClass()->m_lpszClassName);
                        }
                        InsertMenu(menu.GetSubMenu(0)->GetSafeHmenu(), -1, MF_BYPOSITION | MF_STRING, IDS_MULTIOBJECT_NEAR+iSelCnt, szMenuString);
                        pObj = pObj->m_pObjectLink;
                    }
                    g_pMenuSelObject[iSelCnt] = NULL;
                    POINT pt;
                    pt.x = iMouseX;
                    pt.y = iMouseY;
                     ClientToScreen((HWND)(g_pAppCase->GetWndHandle()), &pt);
                    m_bInPopupMenu = true;
                    m_pLastSelectObject = m_pBrushObject;
                    TrackPopupMenu(menu.GetSubMenu(0)->GetSafeHmenu(), TPM_LEFTALIGN, pt.x, pt.y, 0, (HWND)(g_pAppCase->GetWndHandle()), NULL);
                    m_bInPopupMenu = false;
                }
                m_pLastSelectObject = m_pBrushObject;
                m_pBrushObject = NULL;
            }
        }
    }
    return FALSE;
    //unguard;
}

BOOL CBrusherObject::OnBrushMove(int iKey, int iMouseX, int iMouseY)
{
    //guard;
    return TRUE;
    //unguard;
}

void CBrusherObject::OnEndBrush(int iKey, int iMouseX, int iMouseY)
{
    //guard;
    if (!m_bActive) return;
    if (m_eOpType==OOT_MOVE)
    {
        if (!AddObjectToScene(iMouseX, iMouseY))
        {
            return;
        }
        m_bLightObject = false;
        m_pBrushObject = NULL;
    }
    //unguard;
}

BOOL CBrusherObject::AddObjectToScene(int iMouseX, int iMouseY)
{
    if (m_pBrushObject==NULL)
        return FALSE;

    if (!g_pScene->AddObject(m_pBrushObject->Id()))
    {
        RtCoreLog().Info("警告: 物件不能放入场景之中，因为没有检查到物件和场景之间的碰撞.\n");
        return FALSE;
    }

	//-----------------jiang modify----------------//
	CEditorFrame* pEditor = (CEditorFrame*)g_pAppCase;
	if(pEditor->m_bAddNpc)
	{
		
		float mx,my,mz;
		float dx = 0,dy = 0,dz = 0;
		BrushNpcInfo *a = new BrushNpcInfo;
		a->m_NpcModel = m_pBrushObject;
		a->iSerialNum = m_pBrushObject->Id().to_string();
		mx = *(m_pBrushObject->GetMatrix().Position());
		my = *(m_pBrushObject->GetMatrix().Position() + 1);
		mz = *(m_pBrushObject->GetMatrix().Position() + 2);

        
		dx = m_pBrushObject->GetMatrix()._10 * (-1) * 20;
		dy = m_pBrushObject->GetMatrix()._11 * (-1) * 20;
		dz = m_pBrushObject->GetMatrix()._12 * (-1) * 20;

		a->m_NpcDir.x = mx + dx;
        a->m_NpcDir.y = my + dy;
		a->m_NpcDir.z = mz + dz;
	    m_pBrushObject->GetMatrix().FaceToVertex(a->m_NpcDir);
		pEditor->m_NpcBrushList.push_back(*a);
		delete a;
		a = NULL;
     }
	//---------------------------------------------//

    // 计算灯光的影响
    if (m_lObjectType==1) // RtsLight
    {
        RtgVertex3 vPos;
        float fRadius;
        vPos.x = ((RtsLight*)m_pBrushObject)->GetMatrix()._30;
        vPos.y = ((RtsLight*)m_pBrushObject)->GetMatrix()._31;
        vPos.z = ((RtsLight*)m_pBrushObject)->GetMatrix()._32;
        fRadius = ((RtsLight*)m_pBrushObject)->fRange;
        ReComputeLight(vPos, fRadius);
    }else if (m_lObjectType==3) // RtsModel
    {
        ReComputeObjectLight((RtsModel*)m_pBrushObject);
    }


	
    return TRUE;
    //unguard;
}

BOOL CBrusherObject::RemoveObjectFromScene(RtsObject* pObject)
{
    float fRadius;
    RtgVertex3 vPos;
    CHECK(pObject!=NULL);

    BOOL bLight = pObject->IsKindOf(RT_RUNTIME_CLASS(RtsLight));

    if (bLight)
    {
        vPos.x = ((RtsLight*)pObject)->GetMatrix()._30;
        vPos.y = ((RtsLight*)pObject)->GetMatrix()._31;
        vPos.z = ((RtsLight*)pObject)->GetMatrix()._32;
        fRadius = ((RtsLight*)pObject)->fRange;
    }

    g_pScene->RemoveObject(pObject->Id());

    if (bLight)
        ReComputeLight(vPos, fRadius);

	CEditorFrame* pEditor = (CEditorFrame*)g_pAppCase;
	std::list<BrushNpcInfo>::iterator iterobj;
	for( iterobj = pEditor->m_NpcBrushList.begin();iterobj != pEditor->m_NpcBrushList.end();)
	{
         if ((*iterobj).m_NpcModel == pObject)       
	     {
            iterobj = pEditor->m_NpcBrushList.erase(iterobj);
            break;
	     }
          iterobj++;

	}

    return TRUE;

}

bool CBrusherObject::ReComputeLight(RtgVertex3& vPos, float fRadius)
{
    RtsObject* pObj;
    RtsSGrid* pGrid;
    int iTileX, iTileY;
    int x, x0, y0, x1, y1, i;
    set<RtSceneBlock*> setBlock;

    // done
    //g_pScene->IncreaseRenderFlag();

    // 重新计算场景地表的颜色值
    g_pScene->GetTileByPos(vPos, iTileX, iTileY);
    i = fRadius / g_fSceneTileWidth;
    x0 = iTileX-i-1;
    x1 = iTileX+i+1;
    y0 = iTileY-i-1;
    y1 = iTileY+i+1;
    for (; y0<=y1; y0++)
    {
        for (x=x0; x<=x1; x++)
        {
            int iBx, iBy;
            RtSceneBlock* pBlock = g_pScene->GetBlockByTile(x, y0, &iBx, &iBy);
            setBlock.insert(pBlock);
            g_pScene->ComputeVertexColor(x, y0);
        }
    }

    for (set<RtSceneBlock*>::iterator i = setBlock.begin(); i != setBlock.end(); ++i)
        g_pScene->RecreateGrass(*i);

    // 重新计算网格内物件的颜色值 (可以优化)
    i = fRadius / (g_fSceneTileWidth*RTS_BLOCK_GRID_SIZE);
    x0 = (iTileX+g_pScene->GetCenterX()*g_iSceneTileCntX)/RTS_BLOCK_GRID_SIZE-i-1;
    x1 = (iTileX+g_pScene->GetCenterX()*g_iSceneTileCntX)/RTS_BLOCK_GRID_SIZE+i+1;
    y0 = (iTileY+g_pScene->GetCenterY()*g_iSceneTileCntY)/RTS_BLOCK_GRID_SIZE-i-1;
    y1 = (iTileY+g_pScene->GetCenterY()*g_iSceneTileCntY)/RTS_BLOCK_GRID_SIZE+i+1;
    for (; y0<=y1; y0++)
    {
        for (x=x0; x<=x1; x++)
        {
            pGrid = g_pScene->GetGrid(x, y0, true);
            if (pGrid==NULL) continue;
            pGrid->pBlock->SetModifyFlag(true);
            for (i=0; i<pGrid->aryObject.size(); i++)
            {
                pObj = RtObjectManager()->GetObject(pGrid->aryObject[i]);
                if (pObj && pObj->IsKindOf(RT_RUNTIME_CLASS(RtsModel)))
                {
                  //  if (pObj->NeedUpdate())
                    {
                        ReComputeObjectLight((RtsModel*)pObj);
                    }
                }
            }
        }
    }
    return true;
}

bool CBrusherObject::ReComputeObjectLight(RtsModel* pObject)
{
    if (pObject==NULL) return false;
    if (pObject->GetActor()==NULL) return false;
    RtgLightItem* pLights;
    RtgVertex3 vLightOffset(0.f, 0.f, 0.f);
    RtgVertex3 vPos, vExt;

    vPos = pObject->Get_AABBox()->vPos * pObject->GetMatrix();
    vExt = pObject->Get_AABBox()->vExt * pObject->GetModelScale();
    float fObjLen = vExt.Length();
    pLights = g_pScene->GetRangeLights(vPos, 200.f+fObjLen, 0x01);
    pObject->m_aryVc.clear();
    pObject->GetActor()->ComputeVC((RtgMatrix16&)(pObject->GetMatrix()), pObject->m_aryVc, vLightOffset, pLights);
    pObject->GetActor()->SetVC(pObject->m_aryVc);
    return true;
}

bool CBrusherObject::CreateObject(long lType, const char* szFile)
{
    //guard;
    DeleteObject();

    m_pRuntimeClass = NULL;
    switch (lType)
    {
    case 1: // light
        m_pRuntimeClass = RT_RUNTIME_CLASS(RtsLight);
        m_OffsetMatrix = m_OffsetLight;
        m_bLightObject = true;
        break;
    case 2: // audio
        m_pRuntimeClass = RT_RUNTIME_CLASS(RtsAudio3D);
        m_OffsetMatrix = m_OffsetAudio;
        break;
    case 3: // object
        m_pRuntimeClass = RT_RUNTIME_CLASS(RtsModel);
        m_OffsetMatrix = m_OffsetObject;
        break;
    case 4: // firefly
        m_pRuntimeClass = RT_RUNTIME_CLASS(RtsInnerveFireFly);
        m_OffsetMatrix = m_OffsetObject;
        break;
    }

    if (m_pRuntimeClass==NULL)
    {
        return false;
    }

    m_lObjectType = lType;
    m_strObjectFile = szFile;
    m_pBrushObject = RtObjectManager()->GetObject(RtObjectManager()->NewObject(m_pRuntimeClass));//RtsObject::Create(m_pRuntimeClass);
    if (m_pBrushObject==NULL) return false;

    RtgVertex3 c3;
    CEditorFrame* pFrame = (CEditorFrame*)g_pAppCase;
    RtsLight* pLight;
    RtsAudio3D* pAudio;
    RtsModel* pModel;
    RtsInnervation* pInnervation;

    switch (lType)
    {
    case 1: // light
        pLight = (RtsLight*)m_pBrushObject;
        c3.x = GetRValue(pFrame->m_dwCurLightColor)/255.f;
        c3.y = GetGValue(pFrame->m_dwCurLightColor)/255.f;
        c3.z = GetBValue(pFrame->m_dwCurLightColor)/255.f;
        pLight->eMode = RTG_LIGHT_POINT;
        pLight->vDiffuse = c3;
        pLight->fRange = pFrame->m_fCurLightRange;
        pLight->vPosition.Zero();
        pLight->fConstantAttenuation  = pFrame->m_fCurLightConstantAttenuation;
        pLight->fLinearAttenuation    = pFrame->m_fCurLightLinearAttenuation;
        pLight->fQuadraticAttenuation = pFrame->m_fCurLightQuadraticAttenuation;
        pLight->SetShowType(RtGetRender(), RtsObject::SO_SPRITE);
        break;
    case 2: // audio
        pAudio = (RtsAudio3D*)m_pBrushObject;
		pAudio->m_szFileName = m_strObjectFile;
        pAudio->Play();
        pAudio->SetShowType(RtGetRender(), RtsObject::SO_SPRITE);
        break;
    case 3: // object
        break;
    case 4: // RtsInnervation : firefly
        break;
    }

    if (!m_pBrushObject->CreateResource(m_strObjectFile, NULL, true))
    {
        DeleteObject();
        return false;
    }
    
    return true;

}

bool CBrusherObject::DeleteObject()
{
    //guard;
	funguard;
    m_bLightObject = false;
    if (m_pBrushObject)
    {
        RtObjectManager()->DelObject(m_pBrushObject->Id());
     //   RtsObject::Destroy(m_pBrushObject);
        m_pBrushObject = NULL;
    }	
    return true;
    //unguard;
	fununguard;
}

void CBrusherObject::OnBeginRender(RtgDevice* pDevice, RtSceneTool& vScene, RtgVertex3& vPos, int iGX, int iGY)
{
    funguard;
    if (!m_bActive) return;
    if (m_bLightObject && m_pBrushObject)
    {
        RtsLight* pPointLight = (RtsLight*)m_pBrushObject;
        pPointLight->vPosition = vPos;
        pPointLight->vPosition += m_OffsetMatrix.Position();
        RtGetRender()->SetLight(0, pPointLight);
        g_pScene->m_iCustomLightCount = 1;
		//tim.yang  record light matrix
		pPointLight->m_Matrix._30 = pPointLight->vPosition.x;
		pPointLight->m_Matrix._31 = pPointLight->vPosition.y;
		pPointLight->m_Matrix._32 = pPointLight->vPosition.z;
		//end
    }else
    {
		//tim.yang  record audio matrix
		if (m_pBrushObject && m_pBrushObject->IsKindOf(RT_RUNTIME_CLASS(RtsAudio3D)))
		{
			RtsAudio3D* pAudio3D = (RtsAudio3D*)m_pBrushObject;
			pAudio3D->m_Matrix._30 = vPos.x;
			pAudio3D->m_Matrix._31 = vPos.y;
			pAudio3D->m_Matrix._32 = vPos.z + 20;
		}
		//end
        g_pScene->m_iCustomLightCount = 0;
        //g_pDevice->SetLight(1, NULL);
    }
    fununguard;
}

void CBrusherObject::OnEndRender(RtgDevice* pDevice, RtSceneTool& vScene, RtgVertex3& vPos, int iGX, int iGY)
{
    funguard;
    if (!m_bActive) return;
    fununguard;
}

void CBrusherObject::OnFrameMove(float fDiffTime)
{
    if (m_pBrushObject)
        m_pBrushObject->FrameMove(fDiffTime * 1000);
}

void CBrusherObject::OnRender(RtgDevice* pDevice, RtSceneTool& vScene, RtgVertex3& vPos, int iGX, int iGY)
{
    if (m_bInPopupMenu) 
        return;

    RtsObject* pRenderBoundBox = NULL;
    RtgVertex3 v3;
    RtgMatrix16 m16;
    
    if (!m_bActive) 
        return;
    
    if (m_pBrushObject && m_pBrushObject->GetState() == Object_State_Ready)
    {
        CEditorFrame* pFrame = (CEditorFrame*)g_pAppCase;
        m16 = m_OffsetMatrix;

        if (pFrame->m_bCheckSnapToGrid)
        {
            RtgVertex3 vBrushPos;
            vScene.GetPosByTile(iGX, iGY, vBrushPos);
            m16._30 += vBrushPos.x;
            m16._31 += vBrushPos.y;
            m16._32 += vBrushPos.z;
        }
        else
        {
            m16._30 += vPos.x;
            m16._31 += vPos.y;
            m16._32 += vPos.z;
        }

        m_pBrushObject->SetMatrix(m16);
        m_pBrushObject->Render(*(pDevice->m_pCamera), *pDevice, RTGRM_ALL);
        pRenderBoundBox = m_pBrushObject;
    }
    else
    {
        pRenderBoundBox = m_pLastSelectObject;
    }

    if (pRenderBoundBox)
        pDevice->DrawAABBox(pRenderBoundBox->Get_World_AABBox(), 0xff0000ff);

}

void CBrusherObject::OnDrawText(RtgDevice* pDevice)
{
    if (m_eOpType==OOT_APPEND)
    {
        char szOutputString[] = "Left Click, Z+Wheel, X+Wheel, C+Wheel, ESC";
        pDevice->DrawString(
            (pDevice->m_iWndWidth - strlen(szOutputString)*8)/2,
            pDevice->m_iWndHeight*3/4, 0xFFFF0000, szOutputString );
    }else if (m_eOpType==OOT_MOVE)
    {
        char szOutputString[] = "Left Click, Left Drag, Left+Z+Wheel, Left+X+Wheel, Left+C+Wheel, ESC";
        pDevice->DrawString(
            (pDevice->m_iWndWidth - strlen(szOutputString)*8)/2,
            pDevice->m_iWndHeight*3/4, 0xFFFF0000, szOutputString );
    }else if (m_eOpType==OOT_ADJUST)
    {
        char szOutputString[] = "Left, Right, Up, Down, PageUp, PageDown, +, -, {, }, ESC";
        pDevice->DrawString(
            (pDevice->m_iWndWidth - strlen(szOutputString)*8)/2,
            pDevice->m_iWndHeight*3/4, 0xFFFF0000, szOutputString );
    }
}

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/

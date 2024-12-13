
#include "gc_include.h"
#include "gc_synchro_item.h"

/*----------------------------------------------------------------------------
-   GcSynchroItem 角色管理
----------------------------------------------------------------------------*/
RT_IMPLEMENT_DYNAMIC(GcRenderItem, RtsRenderAgent, 0, "gc")

void GcRenderItem::Render(RtgCamera& inCamera, RtgDevice& inDevice, RTGRenderMask eMask) {
    guard;

    m_pGraph->Render(&inDevice, eMask);

    // 显示道具名称
    RtgMatrix16 m16;
    m16                         = *(m_pGraph->GetWorldMatrix());
    RtgVertex3        vPos      = m16.Position();
    const RtgMatrix16 m16Player = *(GetWorld()->m_pPlayer->GetMatrix());
    int               iDistance = sqrt((m16._30 - m16Player._30) * (m16._30 - m16Player._30) +
                                       (m16._31 - m16Player._31) * (m16._31 - m16Player._31));
    if (iDistance < 500)  // 距离远就不显示道具名字
    {
        SItemBase* pItemBase = GetWorld()->m_pItemManager->GetItem(dwTID);
        if (pItemBase) {
            // 道具颜色
            DWORD dwColor = 0x00000000;
            if (color == ItemColor_White) {
                if (pItemBase->suitID != 0)
                    dwColor = 0xff00ff00;
                else
                    dwColor = 0xffffffff;
            } else if (color == ItemColor_Green)
                dwColor = 0xff30e512;  //0xFF6060E3;
            else if (color == ItemColor_Blue)
                dwColor = 0xff00aaff;
            else if (color == ItemColor_Golden)
                dwColor = 0xffd0c002;

            // 道具绑定属性
            string strShowString = pItemBase->name;
            if (cBind == IB_Binded) {
                strShowString += "(";
                strShowString += R(MSG_ITEMBINDED);
                strShowString += ")";
            } else if (cBind == IB_BindWhenEquip) {
                strShowString += "(";
                strShowString += R(MSG_ITEMBIND_WHENEQUIP);
                strShowString += ")";
            } else if (cBind == IB_BindWhenPick) {
                strShowString += "(";
                strShowString += R(MSG_ITEMBIND_WHENPICK);
                strShowString += ")";
            }

            int posX, posY;
            GetDevice()->GetScreenPos(vPos, posX, posY);
            //g_pDevice2D->DrawString(0, strShowString.c_str(), dwColor, posX-60, posY-50, 120, 30, RtwStringFormat(alignCenter, alignCenter));
            //GetDevice()->DrawString(posX-60, posY-50, dwColor, strShowString.c_str());// change [3/16/2009 tooth.shi]
            RtwRect  rect(posX - 60, posY - 35, posX + 60, posY + 50);
            RtwPixel pixel(dwColor);
            g_workspace.getFontManager()->DrawString(strShowString, rect, pixel, false,
                                                     SUiTextAlignment(alignCenter, alignNear));
        }
    }

    unguard;
}

void GcRenderItem::RenderHighLight(RtgCamera& inCamera, RtgDevice& inDevice, RtgVertex3& color,
                                   RtgVertex3& scale) {
    guard;
    m_pGraph->RenderHighLight(&inDevice, color, scale);
    unguard;
}

/*----------------------------------------------------------------------------
-   GcSynchroItem 角色管理
----------------------------------------------------------------------------*/

GcSynchroItem::GcSynchroItem() {
    m_fDownloadTime = 0.f;
}

GcSynchroItem::~GcSynchroItem() {}

bool GcSynchroItem::Init() {
    return true;
}

bool GcSynchroItem::Clear() {
    guard;

    EXT_SPACE::unordered_map<DWORD, SGcsItem*>::iterator it;
    for (it = m_mapItems.begin(); it != m_mapItems.end(); it++) {
        g_pScene->RemoveObjectByLastRenderList(&((*it).second)->render);
        RtcGetActorManager()->ReleaseActor(((*it).second)->pGraph);
        DEL_ONE((*it).second);
    }
    m_mapItems.clear();
    return true;

    unguard;
}

// 开始网络刷新位置等信息
void GcSynchroItem::OnBeginDownloadSnapshot() {
    m_fDownloadTime = GetDevice()->GetAppTime();
}

// 结束网络刷新位置等信息，如果有的物件没有被刷新就会被Disable
void GcSynchroItem::OnEndDownloadSnapshot() {
    guard;

    SGcsItem*                                            pItem;
    std::list<SGcsItem*>                                 delList;
    EXT_SPACE::unordered_map<DWORD, SGcsItem*>::iterator it;
    for (it = m_mapItems.begin(); it != m_mapItems.end(); it++) {
        pItem = (*it).second;
        if (m_fDownloadTime != pItem->fDownloadTime) {
            if (m_fDownloadTime - pItem->fDownloadTime > 5.0f) {
                delList.push_back(pItem);
                // LOG("信息: 一个道具因为长时间没有网络数据被删除了.\n");
            } else {
                pItem->pGraph->Enable(false, false);
                pItem->bDisplay = false;
            }
        }
    }
    std::list<SGcsItem*>::iterator itList;
    for (itList = delList.begin(); itList != delList.end(); itList++) {
        DeleteItem(*itList);
    }

    unguard;
}

void GcSynchroItem::OnDownloadActorSnapshot(DWORD dwOID, DWORD dwTID, BYTE itemColor, BYTE itemBind,
                                            float fX, float fY, float fZ, BYTE cDir,
                                            BYTE dropStyle) {
    guard;

    SGcsItem* pItem = Find(dwOID);
    if (pItem) {
        pItem->fDownloadTime = m_fDownloadTime;
        pItem->pGraph->Enable(true, true);
        pItem->bDisplay = true;
    } else {
        NewItem(dwOID, dwTID, itemColor, itemBind, fX, fY, fZ, cDir, dropStyle);
    }

    unguard;
}

void GcSynchroItem::Run(float fSecond) {
    guard;

    EXT_SPACE::unordered_map<DWORD, SGcsItem*>::iterator it = m_mapItems.begin();
    for (; it != m_mapItems.end(); it++) {
        SGcsItem* pGcsItem = (*it).second;
        if (!pGcsItem->pGraph)
            continue;

        pGcsItem->timeFall -= fSecond;
        RtgMatrix12 m12 = (*pGcsItem->pGraph->GetMatrix());
        if (pGcsItem->timeFall > 0) {
            switch (pGcsItem->dropStyle) {
                case ItemDrop_ParabolaNear: {
                    float tmp = 1 - pGcsItem->timeFall;
                    m12._32 -= tmp * tmp * 100;
                    m12._30 += 12 * tmp * sin(pGcsItem->degree);
                    m12._31 += 12 * tmp * cos(pGcsItem->degree);
                    if (m12._32 <= pGcsItem->zPosGround) {
                        pGcsItem->timeFall = 0;
                        m12._32            = pGcsItem->zPosGround;
                    }
                } break;
                case ItemDrop_ParabolaFar: {
                    float tmp = 1 - pGcsItem->timeFall;
                    m12._32 -= tmp * tmp * 100;
                    m12._30 += 12 * tmp * sin(pGcsItem->degree);
                    m12._31 += 12 * tmp * cos(pGcsItem->degree);
                    if (m12._32 <= pGcsItem->zPosGround) {
                        pGcsItem->timeFall = 0;
                        m12._32            = pGcsItem->zPosGround;
                    }
                } break;
                case ItemDrop_Descend: {
                    float tmp = 1 - pGcsItem->timeFall;
                    m12._32 -= tmp * tmp * 100;
                    if (m12._32 <= pGcsItem->zPosGround) {
                        pGcsItem->timeFall = 0;
                        m12._32            = pGcsItem->zPosGround;
                    }
                } break;
                default:  // 与ItemDrop_ParabolaNear处理相同
                {
                    float tmp = 1 - pGcsItem->timeFall;
                    m12._32 -= tmp * tmp * 100;
                    m12._30 += 12 * tmp * sin(pGcsItem->degree);
                    m12._31 += 12 * tmp * cos(pGcsItem->degree);
                    if (m12._32 <= pGcsItem->zPosGround) {
                        pGcsItem->timeFall = 0;
                        m12._32            = pGcsItem->zPosGround;
                    }
                } break;
            }
        } else {
            m12._32 = pGcsItem->zPosGround;
        }

        pGcsItem->pGraph->SetMatrix(m12);
    }

    unguard;
}

void GcSynchroItem::OnAfterScenePreRender(RtgCamera& inCamera, RtScene* pScene) {
    guard;

    SGcsItem*                                            pItem;
    RtgVertex3                                           vPos;
    RtgMatrix16                                          m16;
    EXT_SPACE::unordered_map<DWORD, SGcsItem*>::iterator it;
    for (it = m_mapItems.begin(); it != m_mapItems.end(); it++) {
        pItem = (*it).second;
        if (pItem->pGraph != NULL && pItem->bDisplay) {
            m16 = *(pItem->pGraph->GetWorldMatrix());
            if (inCamera.CheckVisable(pItem->pGraph->Get_WorldBoundBox())) {
                pScene->AddRenderObject(&(pItem->render));
            }
        }
    }

    unguard;
}

GcSynchroItem::SGcsItem* GcSynchroItem::Find(DWORD dwOID) {
    guard;

    EXT_SPACE::unordered_map<DWORD, SGcsItem*>::iterator it = m_mapItems.find(dwOID);
    if (it != m_mapItems.end()) {
        return (*it).second;
    }
    return NULL;

    unguard;
}

GcSynchroItem::SGcsItem* GcSynchroItem::NewItem(DWORD dwOID, DWORD dwTID, BYTE itemColor,
                                                BYTE itemBind, float fX, float fY, float fZ,
                                                BYTE cDir, BYTE dropStyle) {
    guard;

    SGcsItem* pNew = this->Find(dwOID);
    if (pNew) {
        CHECKEX("该ID的道具已经存在");
        return pNew;
    }
    if (!g_pScene->GetTerrainHeight(fX, fY, fZ)) {
        return NULL;
    }
    pNew                = RT_NEW SGcsItem;
    pNew->dwOID         = dwOID;
    pNew->dwTID         = dwTID;
    pNew->color         = itemColor;
    pNew->cBind         = itemBind;
    pNew->bDisplay      = true;
    pNew->fDownloadTime = m_fDownloadTime;
    pNew->dropStyle     = dropStyle;

    pNew->timeFall          = 1;  // 1 second
    static float lastDegree = 0;
    static float PIDivide3  = PI / 3;
    pNew->degree            = lastDegree + PIDivide3;  //DegreeToRadian(rand()%360);
    if (pNew->degree >= PIx2)
        pNew->degree -= PIx2;
    lastDegree = pNew->degree;

    RtgMatrix16 m16;
    m16.Unit();
    rtgDecodeUprightDirection(m16, cDir);
    m16._30          = fX;
    m16._31          = fY;
    m16._32          = fZ;
    pNew->xPosFall   = m16._30;
    pNew->yPosFall   = m16._31;
    pNew->zPosFall   = m16._32 + 20.0f;
    pNew->zPosGround = m16._32;
    m16._32 += 20.0f;
    //	m16.Scale(4, 4, 4);

    SItemBase* pItem = GetWorld()->m_pItemManager->GetItem(dwTID);
    if (pItem == NULL) {
        ERR1("GcSynchroItem::NewItem 试图创建一个ID不认识的道具[ID=%d].\n", dwTID);
        return NULL;
    }
    pNew->pGraph       = RtcGetActorManager()->CreateActor(pItem->szModel);
    std::string zModel = pItem->szModel;
    P_LOGINFO("GcSynchroItem::NewItem 创建道具" + zModel);
    if (!pNew->pGraph) {

        ERR1("GcSynchroItem::NewItem 创建道具[%s]失败, 无法创建道具模型.\n", pItem->szModel);
        return NULL;
    }

    //// 如果是宝石就播放声音
    //if (ItemIsGem(dwTID))
    //{
    //	RtgMatrix16 m16Player = *(GetWorld()->m_pPlayer->Matrix());
    //	int iDistance = sqrt((m16._30-m16Player._30)*(m16._30-m16Player._30)+(m16._31-m16Player._31)*(m16._31-m16Player._31));
    //	if (iDistance <= 300)
    //	{
    //		RtgVertex3 pos;
    //		g_pSoundMgr->PlayOnce("it011.wav", false, 0, pos);
    //	}
    //}

    const RtgMatrix12* pMat = pNew->pGraph->GetWorldMatrix();
    RtgVertex3         vColor;
    GetWorld()->GetScene()->GetTerrainColor(pMat->_30, pMat->_31, vColor);
    if (vColor.x < 0.3)
        vColor.x = 0.3;
    if (vColor.y < 0.3)
        vColor.y = 0.3;
    if (vColor.z < 0.3)
        vColor.z = 0.3;
    pNew->pGraph->SetBaseColor(vColor);

    pNew->pGraph->SetMatrix(m16);
    //*(pNew->pGraph->GetMatrix()) = m16;
    pNew->render.SetGraph(pNew->pGraph);
    pNew->render.dwOID = dwOID;
    pNew->render.dwTID = dwTID;
    pNew->render.color = itemColor;
    pNew->render.cBind = itemBind;
    m_mapItems.insert(EXT_SPACE::unordered_map<DWORD, SGcsItem*>::value_type(dwOID, pNew));
    return pNew;

    unguard;
}

void GcSynchroItem::DeleteItem(GcSynchroItem::SGcsItem* pItem) {
    guard;
    m_mapItems.erase(pItem->dwOID);
    g_pScene->RemoveObjectByLastRenderList(&pItem->render);
    RtcGetActorManager()->ReleaseActor(pItem->pGraph);
    DEL_ONE(pItem);
    unguard;
}

bool GcSynchroItem::HitTest(RtgVertex3& vOrg, RtgVertex3& vDir, bool bGetNearest, DWORD& outOID,
                            DWORD& outType, DWORD& outColor, DWORD& outBind) {
    guard;

    SGcsItem*                                            pItem;
    SGcsItem*                                            pNearestItem = NULL;
    EXT_SPACE::unordered_map<DWORD, SGcsItem*>::iterator it;
    RtgVertex3                                           v0, v1, v2, v3, vMin, vMax;
    float                                                fDist, r2, fOutMinDist;
    RtgAABB*                                             pAABB;
    RtgMatrix16                                          m16;
    v0 = vOrg;
    v1 = v0 + vDir * 1000.f;  // 只判断1000距离远的物体
    for (it = m_mapItems.begin(); it != m_mapItems.end(); it++) {
        pItem = (*it).second;
        if (!pItem->bDisplay)
            continue;

        pAABB = pItem->pGraph->GetStaticBoundingBox();
        m16   = *(pItem->pGraph->GetWorldMatrix());
        m16.Invert();
        v2   = v0 * m16;
        v3   = v1 * m16;
        vMin = pAABB->Min();
        vMax = pAABB->Max();

        if (rtgIsLineIntersectAABB(v2, v3, vMin, vMax, &fDist, &r2)) {
            if (bGetNearest) {
                outOID   = pItem->dwOID;
                outType  = pItem->dwTID;
                outColor = pItem->color;
                outBind  = pItem->cBind;
                return true;
            }
            if (fDist < 0.f)
                fDist = 0.f;
            if (pNearestItem == NULL || fDist < fOutMinDist) {
                pNearestItem = pItem;
                fOutMinDist  = fDist;
            }
        }
    }
    if (pNearestItem) {
        outOID   = pItem->dwOID;
        outType  = pItem->dwTID;
        outColor = pItem->color;
        outBind  = pItem->cBind;
        return true;
    }
    return false;

    unguard;
}

float GcSynchroItem::_distant(GcSynchroItem::SGcsItem* pItem) {
    return (GetWorld()->m_pPlayer->GetMatrix()->_30 - pItem->xPosFall) *
               (GetWorld()->m_pPlayer->GetMatrix()->_30 - pItem->xPosFall) +
           (GetWorld()->m_pPlayer->GetMatrix()->_31 - pItem->yPosFall) *
               (GetWorld()->m_pPlayer->GetMatrix()->_31 - pItem->yPosFall);
}

bool GcSynchroItem::FindMapItems()  //ac.ma
{
    EXT_SPACE::unordered_map<DWORD, GcSynchroItem::SGcsItem*>::iterator iter  = m_mapItems.begin();
    GcSynchroItem::SGcsItem*                                            pItem = NULL;

    float fDMin = 999999999.f;

    GcSynchroItem::SGcsItem* pTarget = NULL;

    for (; iter != m_mapItems.end(); ++iter) {
        pItem = iter->second;

        //		if(pItem->m_itemColor == 0) continue;

        //		if(!m_bWhitePick && pItem->m_itemColor == 0) continue;
        //		if(!m_bGreenPick && pItem->m_itemColor == 1) continue;
        //		if(!m_bBluePick && pItem->m_itemColor == 2) continue;
        //		if(!m_bGoldPick && pItem->m_itemColor == 3) continue;

        if (_distant(pItem) < fDMin) {
            fDMin   = _distant(pItem);
            pTarget = pItem;
        }
    }

    if (pTarget == NULL)
        return false;  //如果搜索不到指定物品，则return
    if (fDMin > 160000)
        return false;  //物品距离太远，则return。
                       // 	else {
                       // 		PushCallStack(csn_GcActor_AddCommandMoveTo);
                       // 		GetWorld()->m_pPlayer->AddCommandMoveTo(pTarget->xPosFall,pTarget
                       // 		->yPosFall,0,-1.0f,0);
                       // 		PopCallStack();
                       // 	}

    //RtCoreLog().Info("拾取道具ID:%d\n",pTarget->ObjectID);

    CItemNetHelper::NetSend_PickItem(pTarget->dwOID);
    return true;
}

/*----------------------------------------------------------------------------
-   End.
----------------------------------------------------------------------------*/

#include "gc_include.h"
#include "ui_form_minimap.h"
#include "ui_form_task.h"
#include "UIForm_MiddleMap.h"
#include <mmsystem.h>

CUIForm_MiddleMap::CUIForm_MiddleMap(void)
    : m_frmMiddleMap(NULL), m_pcomComboBox(NULL), m_pTaskListUI(NULL), m_pNPCTree(NULL),
      m_pbtnTask(NULL), m_pbtnNPC(NULL), m_plbTasklabel(NULL), m_pPreviewImage(NULL),
      m_pathNode(NULL), m_vPatchPoint(NULL), m_isGetPatchPoint(false), m_wShowFlag(0x007f),
      m_iOffsetX(0), m_iOffsetY(0) {
    guard;
    m_frmMiddleMap = LOAD_UI("fmsysmap");
    m_btnClose     = LOAD_UI_T(RtwButton, "fmsysmap.$hide");
    m_btnClose->EvLClick += RTW_CALLBACK(this, CUIForm_MiddleMap, OnHideMiddleMap);
    m_pfrmMap      = LOAD_UI("fmsysmap.fmborder.Areamap");
    m_pcomComboBox = LOAD_UI_T(RtwComboBox, "fmsysmap.fmchoose.cbtbc");
    m_pTaskListUI  = LOAD_UI_T(RtwListBox, "fmsysmap.tbcchoose.fmAaea1.lbx1");
    //m_pNPCListUI				= LOAD_UI_T(RtwListBox,"fmsysmap.tbcchoose.fmAaea1.lbx1");
    m_pNPCTree = LOAD_UI_T(RtwTree, "fmsysmap.tbcchoose.fmAaea2.trnpclist");
    m_pbtnTask = LOAD_UI_T(RtwButton, "fmsysmap.tbcchoose.btntask");
    m_pbtnNPC  = LOAD_UI_T(RtwButton, "fmsysmap.tbcchoose.btnnpc");
    //m_pbtnUp					= LOAD_UI_T(RtwButton,"fmsysmap.fmborder.Areamap.fmbutton.btnup");
    //m_pbtnDown					= LOAD_UI_T(RtwButton,"fmsysmap.fmborder.Areamap.fmbutton.btndown");
    //m_pbtnLeft					= LOAD_UI_T(RtwButton,"fmsysmap.fmborder.Areamap.fmbutton.btnleft");
    //m_pbtnRight					= LOAD_UI_T(RtwButton,"fmsysmap.fmborder.Areamap.fmbutton.btnright");
    m_pbtnAutoRunToNPC = LOAD_UI_T(RtwButton, "fmsysmap.search1");
    m_pbtnShowTaskInfo = LOAD_UI_T(RtwButton, "fmsysmap.search2");
    m_plbTasklabel     = LOAD_UI("fmsysmap.lbwjs");
    m_ptabControl      = LOAD_UI_T(RtwTab, "fmsysmap.tbcchoose");
    m_ptbxPosX         = LOAD_UI_T(RtwTextBox, "fmsysmap.mapxtext");
    m_ptbxPosY         = LOAD_UI_T(RtwTextBox, "fmsysmap.mapytext");
    m_pbtnAutoMove     = LOAD_UI_T(RtwButton, "fmsysmap.movtoxy");
    m_pcheckButton     = LOAD_UI_T(CUiCheckButton, "fmsysmap.showchoose");

    m_pTaskListUI->SetDrawScrollBarV(true);
    //m_pNPCListUI->SetDrawScrollBarV(true);
    m_pNPCTree->SetAutoScrollV(true);
    m_pNPCTree->SetAutoScrollH(true);

    /* gao 2010.1.15
	原来的中地图事件，主要计算都在此类中，
	*/
    m_pfrmMap->ModifyFlags(wfInput | wfHint, 0);
    //m_pfrmMap->ModifyFlag(0,wfMouseMove);
    m_pfrmMap->EvDraw += RTW_CALLBACK(this, CUIForm_MiddleMap, OnDrawMiddleMap);
    m_pfrmMap->EvLClick += RTW_CALLBACK(this, CUIForm_MiddleMap, OnMoveTo);
    //m_pfrmMap->EvMouseMove				+= RTW_CALLBACK(this, UIFormMiniMap, OnMapMouseMoveEvent);
    //m_pfrmMap->EvHint					+= RTW_CALLBACK(this, CUIForm_MiddleMap, OnHint);
    //m_pfrmMap->EvLClick					+= RTW_CALLBACK(this, UIFormMiniMap, OnAreamapMoveTo);
    //m_pfrmMap->ModifyFlags(0,wfHint);
    m_pcomComboBox->EvSelect += RTW_CALLBACK(this, CUIForm_MiddleMap, OnSelectComboBox);
    m_pcomComboBox->EvUpdateText += RTW_CALLBACK(this, CUIForm_MiddleMap, OnSelectComboBox);
    m_ptabControl->EvChangeSelected += RTW_CALLBACK(this, CUIForm_MiddleMap, OnCheckButtonChanged);

    /*m_pbtnUp->EvMouseCapture			+= RTW_CALLBACK(this, CUIForm_MiddleMap,OnNavigationButtonDown);
	m_pbtnDown->EvMouseCapture			+= RTW_CALLBACK(this, CUIForm_MiddleMap,OnNavigationButtonDown);
	m_pbtnLeft->EvMouseCapture			+= RTW_CALLBACK(this, CUIForm_MiddleMap,OnNavigationButtonDown);
	m_pbtnRight->EvMouseCapture			+= RTW_CALLBACK(this, CUIForm_MiddleMap,OnNavigationButtonDown);*/

    //
    m_pbtnAutoRunToNPC->EvLClick += RTW_CALLBACK(this, CUIForm_MiddleMap, OnAutoToNPC);
    m_pbtnShowTaskInfo->EvLClick += RTW_CALLBACK(this, CUIForm_MiddleMap, OnShowTaskInfo);
    m_pbtnAutoMove->EvLClick += RTW_CALLBACK(this, CUIForm_MiddleMap, OnMoveToXY);
    //
    m_pTaskListUI->EvSelect += RTW_CALLBACK(this, CUIForm_MiddleMap, OnEvSelectList);
    //m_pNPCListUI->EvSelect				+= RTW_CALLBACK(this, CUIForm_MiddleMap,OnEvSelectList);
    m_pNPCTree->EvSelect += RTW_CALLBACK(this, CUIForm_MiddleMap, OnEvSelectList);
    m_frmMiddleMap->EvHide += RTW_CALLBACK(this, CUIForm_MiddleMap, OnHide);
    m_ptbxPosX->SetNumberOnly(true);
    m_ptbxPosY->SetNumberOnly(true);
    m_ptbxPosX->SetCapacity(6);
    m_ptbxPosY->SetCapacity(6);

    // 地图重要数据初始化
    m_fSceneWidth    = g_fSceneBlockWidth;    //100*40=4000
    m_fMinimapWidth  = m_fSceneWidth * 0.5f;  //4000*0.5=2000
    m_fMinimapHeight = m_fSceneWidth * 0.5f;  //

    // 从g_TableScene中去读取所有地图名字，并存储在m_mapMiddleMapName
    g_TableScene.GetAllMiddleMapName(m_mapMiddleMapName);
    m_pcomComboBox->RemoveAllItems();
    int i = 0;
    for (const auto& [key, value] : m_mapMiddleMapName) {
        m_pcomComboBox->AddItem(key);
        m_mapNameToIdx.emplace(key, i++);
        m_vectorSNpc.push_back(RT_NEW vector<SNpc*>);
        m_vectorTaskDesc.push_back(RT_NEW vector<STaskDesc*>);
    }

    g_TableNpc.GetNpcForMiddleMap(m_vectorSNpc, m_mapNameToIdx);
    g_TableTask.GetTaskDescForMiddleMap(m_vectorTaskDesc, m_mapNameToIdx);

    m_bChangeMap     = false;
    m_bMouseRDown    = false;
    m_fRealMapPosX   = 0.f;
    m_fRealMapPosY   = 0.f;
    m_iRealMapWidth  = 0;
    m_iRealMapHeight = 0;
    m_iSelectMapNum  = -1;
    memset(m_fDesPos, 0, sizeof(m_fDesPos));
    m_bNeedShowPoint = false;
    Init();
    unguard;
}

CUIForm_MiddleMap::~CUIForm_MiddleMap(void) {
    guard;
    m_frmMiddleMap->Hide();
    // 释放地图文件名存储文件
    m_mapMiddleMapName.clear();
    map<string, IMAGEINFO*>::iterator it;
    for (it = m_mapMiddleMapImageinfo.begin(); it != m_mapMiddleMapImageinfo.end(); ++it)
        DEL_ONE((*it).second);
    // 因为存储的指针是原有全局变量，所以不必进行释放内存
    for (int i = 0; i < m_vectorSNpc.size(); i++)
        DEL_ONE(m_vectorSNpc[i]);
    for (int i = 0; i < m_vectorTaskDesc.size(); i++)
        DEL_ONE(m_vectorTaskDesc[i]);
    //if (m_ImageAutoFlag)//释放序列帧 add by 李泽华 2010.05.18
    //{
    //	delete m_ImageAutoFlag;
    //	m_ImageAutoFlag = NULL;
    //}
    m_ImageAutoFlag->DropImageList();
    m_mapMiddleMapImageinfo.clear();
    m_vectorNPCLabel.clear();
    m_vectorTaskLabel.clear();
    m_vectorMonsterLabel.clear();
    m_vectorNPCNameLabel.clear();
    m_vectorMonsterNameLabel.clear();
    m_vectorTeamActor.clear();
    //m_mapTaskInfo.clear();
    unguard;
}

void CUIForm_MiddleMap::Show() {
    guard;
    //玩家显示中地图的新手提示
    GetWorld()->ShowMiddleMap();
    OnPreDraw();
    m_frmMiddleMap->Show();
    unguard;
}

void CUIForm_MiddleMap::SetReferencePoint() {
    guard;
    // 获取当前选中地图的 NPC 列表迭代器

    auto& selectedNpcVector = m_vectorSNpc[m_iSelectMapNum];  // 获取选中地图的 vector

    // 如果当前选中地图没有 NPC，则直接返回
    if (selectedNpcVector->empty())
        return;

    // 初始化地图指针和地图区域矩形
    RtsSceneBlockMap* pMap = nullptr;
    RtwRect           RealMapRect(0, 0, 512, 512);
    // 获取 NPC 的位置坐标 (X, Y)
    const long* pPos = (*selectedNpcVector).at(0)->Pos;

    // 计算中心点坐标，坐标分段为 4000 的倍数，并加上 2000 作为偏移量
    float fCenterX = std::floor(static_cast<float>(pPos[0]) / 4000.f) * 4000.f + 2000.f;
    float fCenterY = std::floor(static_cast<float>(pPos[1]) / 4000.f) * 4000.f + 2000.f;
    g_pScene->OffsetToMap(fCenterX, fCenterY);

    // 计算区域的边界
    float fMinX = fCenterX - m_fSceneWidth / 2.f;
    float fMaxX = fCenterX + m_fSceneWidth / 2.f;
    float fMinY = fCenterY - m_fSceneWidth / 2.f;
    float fMaxY = fCenterY + m_fSceneWidth / 2.f;

    // 计算区块索引 (iBx, iBy)
    int iBx = (int)(fMinX / m_fSceneWidth);
    int iBy = (int)(fMinY / m_fSceneWidth);
    // 获取对应的场景区块 (BlockMap)
    pMap = g_pScene->GetBlockMap(iBx - g_pScene->GetCenterX(), iBy - g_pScene->GetCenterY());
    // 如果找到有效的 BlockMap，进一步获取场景信息
    if (pMap && pMap->m_szFileName[0] != 0) {

        SSceneInfo* pSceneInfo = g_TableScene.FindScene(std::string(pMap->m_szFileName));
        if (!pSceneInfo)
            return;
        // 如果场景有中间地图文件名，则继续处理
        if (pSceneInfo->szSceneMiddleMapFileName[0] != 0) {
            // 获取中间地图的宽高及偏移
            int xBlockNum       = pSceneInfo->iMiddleMapWidth;
            int yBlockNum       = pSceneInfo->iMiddleMapHeight;
            int xOffsetBlockNum = pSceneInfo->iMiddleMapWidthOffset;
            int yOffsetBlockNum = pSceneInfo->iMiddleMapHeightOffset;
            // 设置地图区域矩形的宽高
            RealMapRect.right  = xBlockNum * 512;
            RealMapRect.bottom = yBlockNum * 512;
            m_iRealMapWidth    = RealMapRect.right;
            m_iRealMapHeight   = RealMapRect.bottom;
            // 以下计算出当前所在地图的起始坐标
            m_fRealMapPosX = fCenterX - xOffsetBlockNum * 4000 - 2000;
            m_fRealMapPosY = fCenterY - yOffsetBlockNum * 4000 - 2000;
        }
    }
    unguard;
}

void CUIForm_MiddleMap::OnPreDraw() {
    guard;
    auto it = m_mapNameToIdx.find(m_strNowShowMapName);
    if (it != m_mapNameToIdx.end())
        m_iSelectMapNum = it->second;
    else
        return;
    SetReferencePoint();
    int iSelect = m_ptabControl->GetSelectedItem();
    if (iSelect == 0) {
        //// 选择任务信息
        //m_pTaskListUI->RemoveAllItems(true);
        //m_mapTaskInfo.clear();
        //std::vector<STaskDesc>::iterator it;
        //int num = 0;
        //for(it=g_TableTask.GetTaskTable().begin(); it!=g_TableTask.GetTaskTable().end(); it++)
        //{
        //	/*if(it->IsDoing == 0) continue;
        //	if(it->IsDoing == 2) continue;*/
        //	char name[50];
        //	STaskDesc *desc = &(*it);
        //	if(!desc) continue;
        //	if(desc->m_TaskSencenID != m_strNowShowMapName)
        //		continue;
        //	else
        //	{
        //		//添加到相应的list中去，至于分类，当前不需完成
        //		sprintf(name,"%s-%d级",desc->name.c_str(),desc->reqMinLev);
        //		m_pTaskListUI->AddItem(name);
        //		if(GetPlayer()->m_core.Lev - desc->reqMinLev >= 5)
        //			m_pTaskListUI->SetItemColor(num,0xff939393);
        //		else if(GetPlayer()->m_core.Lev - desc->reqMinLev < 0)
        //			m_pTaskListUI->SetItemColor(num,0xffff0000);
        //		else
        //			m_pTaskListUI->SetItemColor(num,0xff00ff00);
        //		m_mapTaskInfo.insert(make_pair(name,desc));
        //		num++;
        //	}
        //}
        //m_pTaskListUI->RemoveAllItems(true);
        //STaskDesc *desc;
        //char name[50];
        //int num = 0;
        //vector<STaskDesc*>::iterator it;
        //for(it= m_vectorTaskDesc[m_iSelectMapNum]->begin();it!= m_vectorTaskDesc[m_iSelectMapNum]->end();it++)
        //{
        //	desc = *it;
        //	sprintf(name,"%s-%d级",desc->name.c_str(),desc->reqMinLev);
        //	m_pTaskListUI->AddItem(name);
        //	if(GetPlayer()->m_core.Lev - desc->reqMinLev >= 5)
        //		m_pTaskListUI->SetItemColor(num,0xff939393);
        //	else if(GetPlayer()->m_core.Lev - desc->reqMinLev < 0)
        //		m_pTaskListUI->SetItemColor(num,0xffff0000);
        //	else
        //		m_pTaskListUI->SetItemColor(num,0xff00ff00);
        //	//m_mapTaskInfo.insert(make_pair(name,desc));
        //	num++;
        //}
        m_pTaskListUI->RemoveAllItems(true);
        g_layerMain->m_formTask->GetTaskByGroup(m_vectorCanAccept);
        vector<STaskDesc*>::iterator it;
        STaskDesc*                   desc;
        int                          num = 0;
        for (it = m_vectorCanAccept.begin(); it != m_vectorCanAccept.end(); it++) {
            desc = *it;
            if (!desc)
                continue;
            if (desc->m_TaskSencenID != m_strNowShowMapName)
                continue;
            m_pTaskListUI->AddItem(desc->name.c_str());
            if (GetPlayer()->m_core.Lev - desc->reqMinLev >= 5)
                m_pTaskListUI->SetItemColor(num, 0xff939393);
            else if (GetPlayer()->m_core.Lev - desc->reqMinLev < 0)
                m_pTaskListUI->SetItemColor(num, 0xffff0000);
            else
                m_pTaskListUI->SetItemColor(num, 0xff00ff00);
            num++;
        }
    } else if (iSelect == 1) {
        // 先择NPC信息
        //m_pNPCListUI->RemoveAllItems(true);
        m_pNPCTree->DeleteAllItems();
        vector<SNpc*>::iterator it;
        vector<string>          Typeid;
        for (it = m_vectorSNpc[m_iSelectMapNum]->begin();
             it != m_vectorSNpc[m_iSelectMapNum]->end(); it++) {
            if ((*it)->TypeId.c_str() == NULL)
                continue;
            vector<std::string>::iterator result =
                find(Typeid.begin(), Typeid.end(), (*it)->TypeId);
            if (result == Typeid.end())
                Typeid.push_back((*it)->TypeId);
        }
        vector<string>::iterator It;
        for (It = Typeid.begin(); It != Typeid.end(); ++It) {
            m_pNPCTree->InsertTreeItem(NULL, (*It).c_str(), "");
            for (it = m_vectorSNpc[m_iSelectMapNum]->begin();
                 it != m_vectorSNpc[m_iSelectMapNum]->end(); it++) {
                if ((*it)->TypeId.c_str() == NULL)
                    continue;
                if (!strcmp((*it)->TypeId.c_str(), (*It).c_str())) {
                    m_pNPCTree->InsertTreeItem((*It).c_str(), (*it)->Name.c_str(), "");
                    m_pNPCTree->SetItemColor((*it)->Name.c_str(), 0xFFFFFFFF);
                }
            }
        }
    }
    unguard;
}

void CUIForm_MiddleMap::Hide() {
    guard;
    m_pcomComboBox->HideDrop();
    m_frmMiddleMap->Hide();
    m_previewLabel->Hide();
    g_workspace.RemoveTopRenderWidget(m_previewLabel->getId());
    unguard;
}

void CUIForm_MiddleMap::OnHide(ui::RtwWidget* sender, RtwEventDelegate* e) {
    guard;
    m_pcomComboBox->HideDrop();
    m_frmMiddleMap->Hide();
    m_previewLabel->Hide();
    g_workspace.RemoveTopRenderWidget(m_previewLabel->getId());
    unguard;
}

void CUIForm_MiddleMap::Init() {
    guard;
    RtwImage* tmpImage;
    tmpImage   = g_workspace.getImageFactory()->createImage("ui\\ui_texture\\pic_d_icon10.gif");
    m_lbPlayer = (RtwLabel*)g_workspace.getWidgetFactory()->createWidget(wtLabel);
    m_lbPlayer->ModifyFlag(wfGrabMouse | wfMouseMove | wfHint, 0);
    if (tmpImage) {
        tmpImage->SetBlend(true);
        m_lbPlayer->SetBackgroundImage(tmpImage);
    }
    m_lbPlayer->SetName("Player_0");
    m_pfrmMap->AddChild(m_lbPlayer);

    m_lbPlayerpreview = (RtwLabel*)g_workspace.getWidgetFactory()->createWidget(wtLabel);
    m_lbPlayerpreview->ModifyFlag(wfGrabMouse | wfMouseMove | wfHint, 0);
    if (tmpImage) {
        tmpImage->SetBlend(true);
        m_lbPlayerpreview->SetBackgroundImage(tmpImage);
    }
    m_lbPlayerpreview->SetName("PlayerPreview");

    tmpImage       = g_workspace.getImageFactory()->createImage("ui\\ui_texture\\outlinedrag.tga");
    m_lbPreviwRect = (RtwLabel*)g_workspace.getWidgetFactory()->createWidget(wtLabel);
    m_lbPreviwRect->ModifyFlag(wfGrabMouse | wfMouseMove | wfHint, 0);
    if (tmpImage) {
        tmpImage->SetBlend(true);
        m_lbPreviwRect->SetBackgroundImage(tmpImage);
    }
    m_lbPreviwRect->SetName("PreviwRect");

    tmpImage = g_workspace.getImageFactory()->createImage("ui\\ui_texture\\minimap_array.dds");
    m_lbPet  = (RtwLabel*)g_workspace.getWidgetFactory()->createWidget(wtLabel);
    m_lbPet->ModifyFlag(wfGrabMouse | wfMouseMove | wfHint, 0);
    if (tmpImage) {
        tmpImage->SetBlend(true);
        m_lbPet->SetBackgroundImage(tmpImage);
    }
    m_lbPet->SetName("Pat_0");
    m_pfrmMap->AddChild(m_lbPet);

    m_ImageAutoFlag = g_workspace.getImageFactory()->createImageSequence();
    RtwImage* pImage;
    char      szBufTemp[128];
    for (int i = 0; i < 23; ++i) {
        rt2_sprintf(szBufTemp, "ui\\ui_texture\\1_%d.dds", i);

        pImage = g_workspace.getImageFactory()->createImage();
        if (pImage->LoadFromFile(szBufTemp)) {
            m_ImageAutoFlag->AddImage(pImage);
            DROP_TRUI_IMAGE(pImage);
        }
    }

    //tmpImage = g_workspace.getImageFactory()->createImage("ui\\ui_texture\\pic_d_icon101.gif");
    m_lbAutoFlag = (RtwLabel*)g_workspace.getWidgetFactory()->createWidget(wtLabel);
    m_lbAutoFlag->ModifyFlag(0, wfGrabMouse | wfMouseMove | wfHint);
    if (m_ImageAutoFlag) {
        m_ImageAutoFlag->SetBlend(true);
        m_lbAutoFlag->SetBackgroundImage(m_ImageAutoFlag);
    }
    m_lbAutoFlag->SetName("AutoFlag_0");
    m_pfrmMap->AddChild(m_lbAutoFlag);

    m_lbNowMousePos = (RtwLabel*)g_workspace.getWidgetFactory()->createWidget(wtLabel);
    m_lbNowMousePos->SetName("NowMousePos_0");
    m_pfrmMap->AddChild(m_lbNowMousePos);
    m_previewLabel = (RtwForm*)g_workspace.getWidgetFactory()->createWidget(wtForm);
    m_previewLabel->SetName("preview");
    g_workspace.AddWidget(m_previewLabel);
    unguard;
}

void CUIForm_MiddleMap::Refresh() {
    guard;
    unguard;
}

bool CUIForm_MiddleMap::IsVisible() {
    guard;
    return m_frmMiddleMap->IsVisible();
    unguard;
}

void CUIForm_MiddleMap::OnDrawMiddleMap(ui::RtwWidget* sender, RtwEventDelegate* e) {
    guard;
    if (GetWorld() == NULL) {
        return;
    }

    RtwRect           mapRect = sender->GetClientRect();
    RtwRect           imgRect = RtwRect(0, 0, mapRect.getWidth(), mapRect.getHeight());
    RtsSceneBlockMap* pMap;
    float             fMinX, fMinY, fMaxX, fMaxY;

    m_iScreenX      = mapRect.left;                                  // 设置屏幕 X 坐标
    m_iScreenY      = mapRect.top;                                   // 设置屏幕 Y 坐标
    m_iScreenWidth  = mapRect.getWidth();                            // 设置屏幕宽度
    m_iScreenHeight = mapRect.getHeight();                           // 设置屏幕高度
    const float* pPos = &(GetWorld()->m_pPlayer->GetMatrix()->_30);  // 获取玩家位置矩阵中的位置
    // 计算玩家当前位置的中心 X 坐标，按 4000 为单位进行对齐

    float fCenterX = floor(pPos[0] / 4000) * 4000 + 2000;
    float fCenterY = floor(pPos[1] / 4000) * 4000 + 2000;

    RtwImage*      pImage = nullptr;
    static RtwRect imgrect;
    // 全局矩形 左上角0，0，右下角为整个地图块的长度，宽度，用来判断怪物或者玩家是否在当前地图
    RtwRect RealMapRect = RtwRect(0, 0, 512, 512);
    // 玩家坐标转化
    float m_fPlayerX = pPos[0];
    float m_fPlayerY = pPos[1];

    // 调用场景对象的 OffsetToMap 方法，将玩家坐标 (m_fPlayerX, m_fPlayerY) 转换为地图坐标
    g_pScene->OffsetToMap(m_fPlayerX, m_fPlayerY);

    if (m_wShowFlag & UIFormMiniMap::EMMSF_SHOW_MAP) {
        g_pScene->OffsetToMap(fCenterX, fCenterY);
        // 计算出周围四个点的位置，并且察看分割方案，同时计算出分割线位置
        fMinX = fCenterX - m_fSceneWidth / 2.f;  // 计算地图区域 左 右 上 下 边界   4000/2    2000
        fMaxX   = fCenterX + m_fSceneWidth / 2.f;
        fMinY   = fCenterY - m_fSceneWidth / 2.f;
        fMaxY   = fCenterY + m_fSceneWidth / 2.f;
        int iBx = (int)(fMinX / m_fSceneWidth);  // 根据左边界计算 X 方向的区块索引
        int iBy = (int)(fMinY / m_fSceneWidth);  // 根据上边界计算 Y 方向的区块索引

        // 获取指定区块的地图  一个地图由四个区块组成  根据坐标获取对应区块
        pMap = g_pScene->GetBlockMap(iBx - g_pScene->GetCenterX(), iBy - g_pScene->GetCenterY());

        if (pMap && pMap->m_szFileName[0] != 0) {
            SSceneInfo* pSceneInfo = g_TableScene.FindScene(std::string(pMap->m_szFileName));
            if (pSceneInfo->szSceneMiddleMapFileName[0] != 0) {  //如果表里的中地图名不为空
                // 将玩家实际所处的地图记录下来，以便于跟玩家要查看的地图进行区分
                m_strNowPlayerRealMapName = pSceneInfo->szMiddleMapName;  //区域名
                if (m_strRealMapOldName !=
                    m_strNowPlayerRealMapName) {  // 如果当前地图与之前的地图不同
                    m_strRealMapOldName = m_strNowPlayerRealMapName;  // 更新旧地图名称
                    m_strNextShowMapName = m_strNowPlayerRealMapName;  // 设置下一个要显示的地图名称
                    m_pcomComboBox->RemoveAllItems();                  // 清空下拉框
                    // 遍历中间地图名称映射表
                    for (auto it = m_mapMiddleMapName.begin(); it != m_mapMiddleMapName.end();
                         ++it) {
                        // 如果中间地图组匹配，则添加到下拉框中
                        if ((it->second).iMiddleMapGroup == pSceneInfo->iMiddleMapGroup)
                            m_pcomComboBox->AddItem(it->first);
                    }
                }
                // 设置实际地图的矩形区域
                RealMapRect.right  = m_iRealMapWidth;
                RealMapRect.bottom = m_iRealMapHeight;
                /* 玩家坐标转化
				说明：0.128 是 512/4000的结果
				乘以0.128是为了将坐标有世界坐标转化到地图上的坐标
				*/
                m_fPlayerX = (m_fPlayerX - m_fRealMapPosX) * 0.128;
                m_fPlayerY = RealMapRect.bottom - (m_fPlayerY - m_fRealMapPosY) * 0.128;

                // 是否重新加载地图
                if (m_bOpenMapFirst ||
                    GetFocusParentWidget(g_workspace.GetFocusWidget()) != m_frmMiddleMap) {
                    m_bOpenMapFirst = false;
                    m_strNextShowMapName =
                        pSceneInfo->szMiddleMapName;  // 设置下一个要显示的地图名称
                    m_pcomComboBox->SetText(m_strNextShowMapName);  // 更新下拉框的文本

                    // 将地图偏移到以玩家为中心的地点
                    m_iOffsetX = (int)(m_fPlayerX - mapRect.getWidth() / 2);  // 计算 X 偏移量
                    if (m_iOffsetX < 0)
                        m_iOffsetX = 0;
                    m_iOffsetY = (int)(m_fPlayerY - mapRect.getHeight() / 2);  // 计算 Y 偏移量
                    if (m_iOffsetY < 0)
                        m_iOffsetY = 0;
                }
            }
        }
        // 只进行渲染地图，而不进行判断玩家位置的操作
        auto it = m_mapMiddleMapName.find(m_strNextShowMapName);
        // 未能在地图里面查找到要加载的地图
        if (it == m_mapMiddleMapName.end() || m_strNextShowMapName == "")
            return;
        else {
            char szName[50];
            if (it->second.szSceneMiddleMapFileName == "") {
                // 说明当前地图没有配中地图
                m_pfrmMap->SetBackgroundImage((RtwImage*)NULL);
                ShowSystemMessage(R(MSG_PICTURE_NOTHASANAME));
                return;
            }
            rt2_sprintf(szName, "ui\\x_textures\\%s.dds",
                        it->second.szSceneMiddleMapFileName.c_str());
            m_pPreviewImage = g_workspace.getImageFactory()->createImage(szName);

            /* gao 2010.1.21
			先从map中去查找，看是否已经创建图片
			*/
            // 先判断当前地图名是否是要加载的如果是，那么就不进行下面的操作
            if (m_strNextShowMapName != m_strNowShowMapName) {
                m_bChangeMap        = true;
                m_strNowShowMapName = it->second.szMiddleMapName;
            }
            map<string, IMAGEINFO*>::iterator itSI;
            itSI = m_mapMiddleMapImageinfo.find(szName);
            if (itSI != m_mapMiddleMapImageinfo.end()) {
                pImage  = (*itSI).second->pImage;
                imgrect = (*itSI).second->rect;

            } else {
                pImage = g_workspace.getImageFactory()->createImage(szName);
                if (pImage) {
                    imgrect.left       = 0;
                    imgrect.top        = 0;
                    imgrect.right      = (*it).second.iMiddleMapWidth * 512;
                    imgrect.bottom     = (*it).second.iMiddleMapHeight * 512;
                    IMAGEINFO* tmpinfo = RT_NEW IMAGEINFO(pImage, &imgrect);
                    m_mapMiddleMapImageinfo.insert(make_pair(szName, tmpinfo));
                } else {
                    ShowSystemMessage(R(MSG_PICTURE_LOADPICTURDFAIL));
                    return;
                }
            }
        }
        // 横向方向的处理
        if (m_iOffsetX >= 0) {
            if (imgrect.right <= imgRect.right + m_iOffsetX)
                m_iOffsetX = imgrect.right - imgRect.right;
        } else if (imgrect.left >= imgRect.left + m_iOffsetX)
            m_iOffsetX = imgrect.left - imgRect.left;
        imgRect.right += m_iOffsetX;
        imgRect.left += m_iOffsetX;
        // 纵向方向的处理
        if (m_iOffsetY >= 0) {
            if (imgrect.bottom <= imgRect.bottom + m_iOffsetY)
                m_iOffsetY = imgrect.bottom - imgRect.bottom;
        } else if (imgrect.top >= imgRect.top + m_iOffsetY)
            m_iOffsetY = imgrect.top - imgRect.top;
        imgRect.bottom += m_iOffsetY;
        imgRect.top += m_iOffsetY;
        if (pImage) {
            pImage->SetSourceRect(imgRect);
            pImage->SetBlend(true);
            m_pfrmMap->SetBackgroundImage(pImage);
        }
    }
    /* end */

    /* gao 2010.1.21
	以下是地图标记部分的渲染
	只有当所渲染地图是玩家所在地图的时候，才进行渲染
	*/
    // 首先清空原来所存储的内容
    m_vecRectInfo.clear();
    int   iX, iY;
    float fX, fY;
    // 如果当前显示的地图跟实际显示的地图相同，那么就进行人物以及怪物的标记渲染
    if (m_strNowPlayerRealMapName == m_strNowShowMapName &&
        (m_wShowFlag & UIFormMiniMap::EMMSF_SHOW_PLAYER)) {
        iX          = m_fPlayerX;
        iY          = m_fPlayerY;
        m_iNpcCount = 0;
        if (imgRect.IsContained(iX, iY)) {
            m_lbPlayer->Show();
            iX                   = iX - m_iOffsetX + mapRect.left;
            iY                   = iY - m_iOffsetY + mapRect.top;
            RtwRect newLabelRect = RtwRect(iX - 4, iY - 4, iX + 3, iY + 3);
            m_lbPlayer->MoveResize(newLabelRect, true);
            m_lbPlayer->SetHintText(GetPlayer()->m_core.Name);
        } else
            m_lbPlayer->Hide();

        // 宠物 签于当前仍久没有完善
        //if (GetWorld() && GetWorld()->m_pPet)
        //{
        //	pPos = &(GetWorld()->m_pPet->GetMatrix()->_30);
        //	fX		= pPos[0];
        //	fY		= pPos[1];
        //	g_pScene->OffsetToMap(fX, fY);
        //	iX		= (fX - m_fRealMapPosX)*0.128;
        //	iY		= RealMapRect.bottom - (fY - m_fRealMapPosY)*0.128;
        //	if(imgRect.IsContained(iX,iY))
        //	{
        //		m_lbPet->Show();
        //		iX = iX - m_iOffsetX + mapRect.left;
        //		iY = iY - m_iOffsetY + mapRect.top;
        //		RtwRect newLabelRect= RtwRect(iX-4,iY-4,iX +3,iY+3);
        //		m_lbPet->MoveResize(newLabelRect,true);
        //	}else
        //		m_lbPet->Hide();
        //	//m_ptNpcs[m_iNpcCount].x = iX;
        //	//m_ptNpcs[m_iNpcCount].y = iY;
        //	//m_ptNpcs[m_iNpcCount].id = GetWorld()->m_pPet->ID();
        //	//m_iNpcCount ++;
        //}else
        //	m_lbPet->Hide();
    } else
        m_lbPlayer->Hide();

    char strId[32];
    // 以下是NPC图片的加载
    map<string, int>::iterator it = m_mapNameToIdx.find(m_strNowShowMapName);
    if (it != m_mapNameToIdx.end())
        m_iSelectMapNum = it->second;
    else
        return;
    //下面的东东是用来加载队友滴图片滴，哈哈  添加者：李泽华
    STeamData* teamData = GetWorld()->m_Team.mData.GetTeamData();
    if (GetWorld()->m_Team.mData.Size()) {
        int teamActor = 0;
        for (; teamActor < GetWorld()->m_Team.mData.Size(); ++teamActor) {

            /*if (teamData[teamActor].mRegion == m_strNowShowMapName)
			{*/
            fX = teamData[teamActor].X;
            fY = teamData[teamActor].Y;
            g_pScene->OffsetToMap(fX, fY);
            iX = (fX - m_fRealMapPosX) * 0.128;
            iY = RealMapRect.bottom - (fY - m_fRealMapPosY) * 0.128;
            RtwLabel* lbTeam;
            if (teamActor >= m_vectorTeamActor.size()) {
                RtwImage* tmpImage = NULL;
                if (GetWorld()->m_Team.mData.GetCaptain() == GetPlayer()->DBID())
                    tmpImage = g_workspace.getImageFactory()->createImage(
                        "ui\\ui_texture\\pic_d_icon04.gif");
                else
                    tmpImage = g_workspace.getImageFactory()->createImage(
                        "ui\\ui_texture\\pic_d_icon05.gif");
                lbTeam = (RtwLabel*)g_workspace.getWidgetFactory()->createWidget(wtLabel);
                rt2_sprintf(strId, "lbTeam_%d", teamActor);
                lbTeam->SetName(strId);
                lbTeam->ModifyFlag(wfGrabMouse | wfMouseMove | wfHint, 0);
                if (tmpImage) {
                    tmpImage->SetBlend(true);
                    lbTeam->SetBackgroundImage(tmpImage);
                }
                m_vectorTeamActor.push_back(lbTeam);
                m_pfrmMap->AddChild(lbTeam);
                lbTeam->SetHintText(teamData[teamActor].mName);
            } else {
                lbTeam = m_vectorTeamActor[teamActor];
                lbTeam->SetHintText(teamData[teamActor].mName);
            }
            if (imgRect.IsContained(iX, iY)) {
                g_workspace.BringWidgetToTop(lbTeam);
                lbTeam->Show();
                iX                   = iX - m_iOffsetX + mapRect.left;
                iY                   = iY - m_iOffsetY + mapRect.top;
                RtwRect newLabelRect = RtwRect(iX - 5, iY - 5, iX + 4, iY + 4);
                lbTeam->MoveResize(newLabelRect, true);
            } else
                lbTeam->Hide();
            /*} */
        }
        while (teamActor < m_vectorTeamActor.size()) {
            m_vectorTeamActor[teamActor]->Hide();
            teamActor++;
        }
    } else {
        int teamsize = 0;
        while (teamsize < m_vectorTeamActor.size()) {
            m_vectorTeamActor[teamsize]->Hide();
            ++teamsize;
        }
    }

    vector<STaskDesc*>::iterator itTaskdesc;
    int                          Taskdescnum = 0;
    STaskDesc*                   desc        = NULL;
    for (itTaskdesc = m_vectorCanAccept.begin(); itTaskdesc != m_vectorCanAccept.end();
         itTaskdesc++) {
        //if((*itTaskdesc)->IsDoing == 0 || (*itTaskdesc)->IsDoing == 2)
        //	continue;
        desc = *itTaskdesc;
        if (!desc)
            continue;
        if (desc->m_TaskSencenID != m_strNowShowMapName)
            continue;
        fX = (*itTaskdesc)->m_posTaskBegin[0] /**0.05*/;
        fY = (*itTaskdesc)->m_posTaskBegin[1] /**0.05*/;
        g_pScene->OffsetToMap(fX, fY);
        iX = (fX - m_fRealMapPosX) * 0.128;
        iY = RealMapRect.bottom - (fY - m_fRealMapPosY) * 0.128;
        // 如果 NPC的 数目超过容器内Lalel数目
        RtwLabel* lbTaskdesc;
        if (Taskdescnum >= m_vectorTaskLabel.size()) {
            RtwImage* tmpImage =
                g_workspace.getImageFactory()->createImage("ui\\ui_texture\\pic_d_icon02.gif");
            lbTaskdesc = (RtwLabel*)g_workspace.getWidgetFactory()->createWidget(wtLabel);
            rt2_sprintf(strId, "lbTaskdesc_%d", Taskdescnum);
            //			lbTaskdesc->CloneFrom(m_lbPet);
            // 			lbTaskdesc->SetName(strId);
            // 			lbTaskdesc->ModifyFlag(0,wfGrabMouse|wfMouseMove|wfHint);
            if (tmpImage) {
                tmpImage->SetBlend(true);
                lbTaskdesc->SetBackgroundImage(tmpImage);
            }
            m_vectorTaskLabel.push_back(lbTaskdesc);
            m_pfrmMap->AddChild(lbTaskdesc);
        } else {
            lbTaskdesc = m_vectorTaskLabel[Taskdescnum];
        }
        if (imgRect.IsContained(iX, iY)) {
            g_workspace.BringWidgetToTop(lbTaskdesc);
            lbTaskdesc->Show();
            iX                   = iX - m_iOffsetX + mapRect.left;
            iY                   = iY - m_iOffsetY + mapRect.top;
            RtwRect newLabelRect = RtwRect(iX - 5, iY - 5, iX + 4, iY + 4);
            lbTaskdesc->MoveResize(newLabelRect, true);
        } else
            lbTaskdesc->Hide();
        //m_ptNpcs[m_iNpcCount].x = iX;
        //m_ptNpcs[m_iNpcCount].y = iY;
        //m_ptNpcs[m_iNpcCount].id = GetWorld()->m_pPet->ID();
        //m_iNpcCount ++;
        Taskdescnum++;
    }

    vector<SNpc*>::iterator itNPC;
    int                     NPCnum = 0, Monsternum = 0;
    for (itNPC = m_vectorSNpc[m_iSelectMapNum]->begin();
         itNPC != m_vectorSNpc[m_iSelectMapNum]->end(); itNPC++) {
        fX = (*itNPC)->Pos[0] /**0.05*/;
        fY = (*itNPC)->Pos[1] /**0.05*/;
        g_pScene->OffsetToMap(fX, fY);
        iX = (fX - m_fRealMapPosX) * 0.128;
        iY = RealMapRect.bottom - (fY - m_fRealMapPosY) * 0.128;
        // 如果 NPC的 数目超过容器内Lalel数目
        if ((*itNPC)->Type != 1) {
            RtwLabel* lbNPC;
            RtwLabel* lbNPCName;
            if (NPCnum >= m_vectorNPCLabel.size()) {
                RtwImage* tmpImage =
                    g_workspace.getImageFactory()->createImage("ui\\ui_texture\\pic_d_icon01.dds");
                lbNPC = (RtwLabel*)g_workspace.getWidgetFactory()->createWidget(wtLabel);
                rt2_sprintf(strId, "lbNPC_%d", NPCnum);
                lbNPC->SetName(strId);
                lbNPC->ModifyFlag(wfGrabMouse | wfMouseMove | wfHint, 0);

                lbNPCName = (RtwLabel*)g_workspace.getWidgetFactory()->createWidget(wtLabel);
                rt2_sprintf(strId, "lbNPCName_%d", NPCnum);
                lbNPCName->SetName(strId);
                lbNPCName->ModifyFlags(0, wfPick);
                if (tmpImage) {
                    tmpImage->SetBlend(true);
                    lbNPC->SetBackgroundImage(tmpImage);
                }
                m_vectorNPCLabel.push_back(lbNPC);
                m_pfrmMap->AddChild(lbNPC);
                lbNPC->SetHintText((*itNPC)->Name);

                m_vectorNPCNameLabel.push_back(lbNPCName);
                m_pfrmMap->AddChild(lbNPCName);
                lbNPCName->SetText((*itNPC)->Name);
            } else {
                lbNPC = m_vectorNPCLabel[NPCnum];
                lbNPC->SetHintText((*itNPC)->Name);

                lbNPCName = m_vectorNPCNameLabel[NPCnum];
                lbNPCName->SetText((*itNPC)->Name);
            }
            if (imgRect.IsContained(iX, iY)) {
                lbNPC->Show();
                iX                   = iX - m_iOffsetX + mapRect.left;
                iY                   = iY - m_iOffsetY + mapRect.top;
                RtwRect newLabelRect = RtwRect(iX - 4, iY - 4, iX + 3, iY + 3);
                lbNPC->MoveResize(newLabelRect, true);
                if (m_pcheckButton->GetChecked()) {
                    lbNPCName->Show();
                    int length = g_workspace.getFontManager()->GetStringWidth((*itNPC)->Name) +
                                 (RtwTextBox::m_LeftSpace << 1) + 3;
                    int height =
                        g_workspace.getFontManager()->getFontSize() + RtwTextBox::m_LineSpace;
                    newLabelRect =
                        RtwRect(iX - length * 0.5, iY - 4 - height, iX + length * 0.5, iY);
                    lbNPCName->MoveResize(newLabelRect, true);
                } else
                    lbNPCName->Hide();
            } else {
                lbNPC->Hide();
                lbNPCName->Hide();
            }
            //m_ptNpcs[m_iNpcCount].x = iX;
            //m_ptNpcs[m_iNpcCount].y = iY;
            //m_ptNpcs[m_iNpcCount].id = GetWorld()->m_pPet->ID();
            lbNPC->EvLClick += RTW_CALLBACK(this, CUIForm_MiddleMap, OnMoveToNPC);
            //m_iNpcCount ++;
            NPCnum++;
        } else {
            RtwLabel* lbMonster;
            RtwLabel* lbMonsterName;
            if (Monsternum >=
                m_vectorMonsterLabel.size()) {  // 检查当前的怪物数量是否大于已创建的标签数量
                RtwImage* tmpImage =
                    g_workspace.getImageFactory()->createImage("ui\\ui_texture\\pic_d_icon03.dds");
                lbMonster = (RtwLabel*)g_workspace.getWidgetFactory()->createWidget(wtLabel);
                rt2_sprintf(strId, "lbMonster_%d", Monsternum);
                lbMonster->SetName(strId);
                lbMonster->ModifyFlag(wfGrabMouse | wfMouseMove | wfHint, 0);

                lbMonsterName = (RtwLabel*)g_workspace.getWidgetFactory()->createWidget(wtLabel);
                rt2_sprintf(strId, "lbNPCName_%d", Monsternum);
                lbMonsterName->SetName(strId);
                lbMonsterName->ModifyFlags(0, wfPick);
                if (tmpImage) {
                    tmpImage->SetBlend(true);
                    lbMonster->SetBackgroundImage(tmpImage);
                }
                m_vectorMonsterLabel.push_back(lbMonster);
                m_pfrmMap->AddChild(lbMonster);
                char lv[6];
                rt2_sprintf(lv, " LV%d", (*itNPC)->Level);
                lbMonster->SetHintText((*itNPC)->Name + lv);

                m_vectorMonsterNameLabel.push_back(lbMonsterName);
                m_pfrmMap->AddChild(lbMonsterName);
                lbMonsterName->SetText((*itNPC)->Name + lv);
            } else {
                lbMonster = m_vectorMonsterLabel[Monsternum];
                char lv[6];
                rt2_sprintf(lv, " LV%d", (*itNPC)->Level);
                lbMonster->SetHintText((*itNPC)->Name + lv);

                lbMonsterName = m_vectorMonsterNameLabel[Monsternum];
                lbMonsterName->SetText((*itNPC)->Name + lv);
            }
            if (imgRect.IsContained(iX, iY)) {
                lbMonster->Show();
                iX                   = iX - m_iOffsetX + mapRect.left;
                iY                   = iY - m_iOffsetY + mapRect.top;
                RtwRect newLabelRect = RtwRect(iX - 4, iY - 4, iX + 3, iY + 3);
                lbMonster->MoveResize(newLabelRect, true);
                if (m_pcheckButton->GetChecked()) {
                    lbMonsterName->Show();
                    int length =
                        g_workspace.getFontManager()->GetStringWidth(lbMonsterName->getText()) +
                        (RtwTextBox::m_LeftSpace << 1) + 3;
                    int height =
                        g_workspace.getFontManager()->getFontSize() + RtwTextBox::m_LineSpace;
                    newLabelRect =
                        RtwRect(iX - length * 0.5, iY - 4 - height, iX + length * 0.5, iY);
                    lbMonsterName->MoveResize(newLabelRect, true);
                } else
                    lbMonsterName->Hide();
            } else {
                lbMonster->Hide();
                lbMonsterName->Hide();
            }
            //m_ptNpcs[m_iNpcCount].x = iX;
            //m_ptNpcs[m_iNpcCount].y = iY;
            //m_ptNpcs[m_iNpcCount].id = GetWorld()->m_pPet->ID();
            lbMonster->EvLClick += RTW_CALLBACK(this, CUIForm_MiddleMap, OnMoveToNPC);
            //m_iNpcCount ++;
            Monsternum++;
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if (!m_isGetPatchPoint)
        GetPatchPoint();
    if ((GetPlayer()->mBaseActor.IsMoveTo()))
        if (m_vPatchPoint.size())
            m_vPatchPoint.clear();
    vector<PatchPoint>::reverse_iterator itPoint;
    for (itPoint = m_vPatchPoint.rbegin(); itPoint != m_vPatchPoint.rend(); ++itPoint) {
        if ((*itPoint).beFound == true) {
            while (itPoint != m_vPatchPoint.rend()) {
                (*itPoint).beFound = true;
                ++itPoint;
            }
            break;
        }
    }
    int                          pointnum = 0;
    vector<PatchPoint>::iterator itPatchPoint;
    for (itPatchPoint = m_vPatchPoint.begin(); itPatchPoint != m_vPatchPoint.end();
         ++itPatchPoint) {
        iX = ((*itPatchPoint).x - m_fRealMapPosX) * 0.128;
        iY = RealMapRect.bottom - ((*itPatchPoint).y - m_fRealMapPosY) * 0.128;
        RtwLabel* lbPoint;
        if (pointnum >= m_vectorPatchPoint.size()) {
            RtwImage* tmpImage =
                g_workspace.getImageFactory()->createImage("ui\\ui_texture\\seekingpoint.dds");
            lbPoint = (RtwLabel*)g_workspace.getWidgetFactory()->createWidget(wtLabel);
            rt2_sprintf(strId, "lbPoint_%d", pointnum);
            lbPoint->SetName(strId);
            lbPoint->ModifyFlag(wfGrabMouse | wfMouseMove, 0);
            if (tmpImage) {
                tmpImage->SetBlend(true);
                lbPoint->SetBackgroundImage(tmpImage);
            }
            m_vectorPatchPoint.push_back(lbPoint);
            m_pfrmMap->AddChild(lbPoint);
        } else {
            lbPoint = m_vectorPatchPoint[pointnum];
        }
        if (imgRect.IsContained(iX, iY) && !((*itPatchPoint).beFound)) {
            g_workspace.BringWidgetToTop(lbPoint);
            lbPoint->Show();
            iX                   = iX - m_iOffsetX + mapRect.left;
            iY                   = iY - m_iOffsetY + mapRect.top;
            RtwRect newLabelRect = RtwRect(iX - 2, iY - 2, iX + 1, iY + 1);
            lbPoint->MoveResize(newLabelRect, true);
            (*itPatchPoint).beFound = PatchPointIsFound(*itPatchPoint);
        } else
            lbPoint->Hide();
        ++pointnum;
    }
    while (pointnum < m_vectorPatchPoint.size()) {
        m_vectorPatchPoint[pointnum]->Hide();
        ++pointnum;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    while (NPCnum < m_vectorNPCLabel.size()) {
        m_vectorNPCLabel[NPCnum]->Hide();
        m_vectorNPCNameLabel[NPCnum]->Hide();
        NPCnum++;
    }
    while (Monsternum < m_vectorMonsterLabel.size()) {
        m_vectorMonsterLabel[Monsternum]->Hide();
        m_vectorMonsterNameLabel[Monsternum]->Hide();
        Monsternum++;
    }

    while (Taskdescnum < m_vectorTaskLabel.size()) {
        m_vectorTaskLabel[Taskdescnum]->Hide();
        Taskdescnum++;
    }
    // 寻路目的地
    if (m_bNeedShowPoint) {
        fX = m_fDesPos[0];
        fY = m_fDesPos[1];

        g_pScene->OffsetToMap(fX, fY);
        iX = (fX - m_fRealMapPosX) * 0.128;
        iY = RealMapRect.bottom - (fY - m_fRealMapPosY) * 0.128;
        if (imgRect.IsContained(iX, iY) && GetWorld()->CanMove(true)) {
            m_lbAutoFlag->Show();
            iX                   = iX - m_iOffsetX + mapRect.left;
            iY                   = iY - m_iOffsetY + mapRect.top;
            RtwRect newLabelRect = RtwRect(iX - 30, iY - 48, iX + 30, iY + 12);
            m_lbAutoFlag->MoveResize(newLabelRect, true);
        } else
            m_lbAutoFlag->Hide();
        // 到达目的地周围
        if (abs(pPos[0] - m_fDesPos[0]) <= 30 && abs(pPos[1] - m_fDesPos[1]) <= 30) {
            m_bNeedShowPoint = false;
            m_lbAutoFlag->Hide();
            if (m_vPatchPoint.size())
                m_vPatchPoint.clear();
        }
    } else
        m_lbAutoFlag->Hide();

    if (mapRect.IsContain(g_workspace.GetMousePosX(), g_workspace.GetMousePosY()) &&
        !m_bMouseRDown) {
        int   mouseX = g_workspace.GetMousePosX();
        int   mouseY = g_workspace.GetMousePosY();
        float fPosX  = mouseX + m_iOffsetX - mapRect.left;
        float fPosY  = mouseY + m_iOffsetY - mapRect.top;
        fPosX        = 7.8125 * fPosX + m_fRealMapPosX;
        fPosY        = (m_iRealMapHeight - fPosY) * 7.8125 + m_fRealMapPosY;
        g_pScene->OffsetToWorld(fPosX, fPosY);
        char POS[20];
        rt2_sprintf(POS, "%d,%d", int(fPosX * 0.05), int(fPosY * 0.05));

        m_lbNowMousePos->Show();
        m_lbNowMousePos->SetText(POS);
        int length =
            g_workspace.getFontManager()->GetStringWidth(POS) + (RtwTextBox::m_LeftSpace << 1) + 3;
        int     height = g_workspace.getFontManager()->getFontSize() + RtwTextBox::m_LineSpace;
        RtwRect newLabelRect = RtwRect(mouseX - length * 0.5, mouseY + 25, mouseX + length * 0.5,
                                       mouseY + 25 + height);
        m_lbNowMousePos->MoveResize(newLabelRect, true);
    } else {
        m_lbNowMousePos->Hide();
    }

    if (m_bChangeMap) {
        OnPreDraw();
        m_bChangeMap = false;
    }
    ShowPreview();
    unguard;
}

void CUIForm_MiddleMap::OnSelectComboBox(RtwWidget* sender, RtwEventDelegate* e) {
    guard;
    int iselect = m_pcomComboBox->GetSelectedItem();
    if (iselect != -1) {
        m_strNextShowMapName = m_pcomComboBox->getSelectItem();
    }
    unguard;
}

void CUIForm_MiddleMap::OnMouseRDrag(int x, int y, int increaseX, int increaseY) {
    guard;
    //玩家显示中地图寻路
    GetWorld()->ShowMiddleMapAI();
    RtwRect mapRealRect = m_pfrmMap->GetClientRect();
    if (mapRealRect.IsContained(g_workspace.GetMousePosX(), g_workspace.GetMousePosY())) {
        // 当右键拖动时候，鼠标在显示地图的区域内时
        m_iOffsetX += -increaseX;
        m_iOffsetY += -increaseY;
    }
    m_bMouseRDown = true;
    unguard;
}

void CUIForm_MiddleMap::OnHideMiddleMap(RtwWidget* sender, RtwEventDelegate* e) {
    if (g_layerMain->m_formGuide1->IsVisible()) {
        g_layerMain->m_formGuide1->Hide();
    }
    m_frmMiddleMap->Hide();
}

void CUIForm_MiddleMap::OnMoveTo(ui::RtwWidget* sender, RtwEventDelegate* e) {
    guard;
    if (GetPlayer()->m_bPShop || !GetWorld()->CanMove(true) ||
        GetPlayer()->GetAutoState())  //change by yz 2010-6-21:自动战斗时阻止自动寻路
        return;
    g_workspace.BringWidgetToBottom(sender);
    RtwRect mapRect = sender->GetClientRect();
    float   fPosX   = e->mouse.x + m_iOffsetX - mapRect.left;
    float   fPosY   = e->mouse.y + m_iOffsetY - mapRect.top;
    fPosX           = 7.8125 * fPosX + m_fRealMapPosX;
    fPosY           = (m_iRealMapHeight - fPosY) * 7.8125 + m_fRealMapPosY;
    g_pScene->OffsetToWorld(fPosX, fPosY);

    SetOnMoveto(fPosX * 0.05, fPosY * 0.05);

    PushCallStack(csn_GcActor_AddCommandMoveTo);
    (((GetWorld()->m_iCheckPlayerSeed = GetWorld()->m_iCheckPlayerSeed * 203014L + 3521051L) >>
      16) &
     0x7fff);
    GetWorld()->m_pPlayer->AddCommandMoveTo(fPosX, fPosY, 0, -1.0f, 0);
    GetWorld()->m_iCheckPlayerSeedSave = GetWorld()->m_iCheckPlayerSeed;
    PopCallStack();
    unguard;
}

void CUIForm_MiddleMap::OnMoveToXY(ui::RtwWidget* sender, RtwEventDelegate* e) {
    guard;
    if (GetPlayer()->m_bPShop || !GetWorld()->CanMove(true) ||
        GetPlayer()->GetAutoState())  //change by yz 2010-6-21:自动战斗时阻止自动寻路
        return;
    if (m_ptbxPosX->getText() == "" || m_ptbxPosY->getText() == "")
        return;
    int iPosX = atoi(m_ptbxPosX->getText().c_str());
    int iPosY = atoi(m_ptbxPosY->getText().c_str());
    GetWorld()->m_pPlayer->GotoThere(iPosX, iPosY);
    SetOnMoveto(iPosX, iPosY);
    unguard;
}

void CUIForm_MiddleMap::OnMoveToNPC(ui::RtwWidget* sender, RtwEventDelegate* e) {
    guard;
    if (GetPlayer()->m_bPShop || !GetWorld()->CanMove(true) ||
        GetPlayer()->GetAutoState())  //change by yz 2010-6-21:自动战斗时阻止自动寻路
        return;
    g_workspace.BringWidgetToBottom(sender);
    RtwRect mapRect = m_pfrmMap->GetFrameRect();

    float fPosX = e->mouse.x + m_iOffsetX - mapRect.left;
    float fPosY = e->mouse.y + m_iOffsetY - mapRect.top;
    fPosX       = 7.8125 * fPosX + m_fRealMapPosX;
    fPosY       = (m_iRealMapHeight - fPosY) * 7.8125 + m_fRealMapPosY;
    g_pScene->OffsetToWorld(fPosX, fPosY);

    SetOnMoveto(fPosX * 0.05, fPosY * 0.05);

    PushCallStack(csn_GcActor_AddCommandMoveTo);
    (((GetWorld()->m_iCheckPlayerSeed = GetWorld()->m_iCheckPlayerSeed * 203014L + 3521051L) >>
      16) &
     0x7fff);
    GetWorld()->m_pPlayer->AddCommandMoveTo(fPosX, fPosY, 0, -1.0f, 0);
    GetWorld()->m_iCheckPlayerSeedSave = GetWorld()->m_iCheckPlayerSeed;
    PopCallStack();
    unguard;
}

RtwWidget* CUIForm_MiddleMap::GetFocusParentWidget(RtwWidget* son) {
    guard;
    RtwWidget* parent = son;
    if (son) {
        while (parent->GetParent() != NULL) {
            parent = parent->GetParent();
        }
    } else
        return NULL;
    return parent;
    unguard;
}

//void CUIForm_MiddleMap::OnNavigationButtonDown(RtwWidget* sender, RtwEventDelegate* e)
//{
//	guard;
//	/*if(sender == m_pbtnUp)
//	{
//		m_iOffsetY -= 2;
//		if(m_iOffsetY < 0)
//			m_iOffsetY = 0;
//	}else if(sender == m_pbtnDown)
//	{
//		m_iOffsetY += 2;
//		if(m_iOffsetY > m_iRealMapHeight - m_iScreenHeight)
//			m_iOffsetY = m_iRealMapHeight - m_iScreenHeight;
//	}else if(sender == m_pbtnLeft)
//	{
//		m_iOffsetX -= 2;
//		if(m_iOffsetX < 0)
//			m_iOffsetX = 0;
//	}else if(sender == m_pbtnRight)
//	{
//		m_iOffsetX += 2;
//		if(m_iOffsetX > m_iRealMapWidth - m_iScreenWidth)
//			m_iOffsetX = m_iRealMapWidth - m_iScreenWidth;
//	}*/
//	unguard;
//}
void CUIForm_MiddleMap::OnCheckButtonChanged(ui::RtwWidget* sender, RtwEventDelegate* e) {
    guard;
    Show();
    unguard;
}

void CUIForm_MiddleMap::OnAutoToNPC(ui::RtwWidget* sender, RtwEventDelegate* e) {
    guard;
    if (GetPlayer()->m_bPShop || !GetWorld()->CanMove(true) ||
        GetPlayer()->GetAutoState())  //change by yz 2010-6-21:自动战斗时阻止自动寻路
        return;
    int iSelect = m_ptabControl->GetSelectedItem();
    if (iSelect == -1) {
        ShowSystemMessage(R(MSG_SELECTAPLACEFIRST));
        return;
    } else if (iSelect == 0) {
        int ilistSelect = m_pTaskListUI->GetSelectedItem();
        if (ilistSelect == -1) {
            ShowSystemMessage(R(MSG_TASK_SELECTATASKFIRST));
            return;
        }
        string                       taskName = m_pTaskListUI->GetItemText(ilistSelect, 0);
        vector<STaskDesc*>::iterator it;
        STaskDesc*                   desc;
        for (it = m_vectorCanAccept.begin(); it != m_vectorCanAccept.end(); it++) {
            desc = *it;
            if (!desc)
                continue;
            if (desc->name == taskName) {
                SetOnMoveto(desc->m_posTaskBegin[0], desc->m_posTaskBegin[1]);
                float iPosX = desc->m_posTaskBegin[0] * 0.05 - 1;
                float iPosY = desc->m_posTaskBegin[1] * 0.05 - 1;
                GetWorld()->m_pPlayer->GotoThere(iPosX, iPosY);
                return;
            }
        }
    } else if (iSelect == 1) {
        RtwTree::Item* mRootItem = m_pNPCTree->getRootItem();
        if (mRootItem->pFirstChild == NULL)
            return;
        RtwTree::Item* pSelectItem = m_pNPCTree->GetLastSelect();
        if (pSelectItem == NULL) {
            ShowSystemMessage(R(MSG_SELECTANPCFIRST));
            return;
        }
        //int ilistSelect = m_pNPCListUI->GetSelectedItem();
        //int ilistSelect = m_pNPCTree->getSelectItem()->text;
        //if(ilistSelect== -1)
        //{
        //	ShowSystemMessage("请先选择一个NPC！");
        //	return;
        //}
        vector<SNpc*>::iterator itNPC;
        for (itNPC = m_vectorSNpc[m_iSelectMapNum]->begin();
             itNPC != m_vectorSNpc[m_iSelectMapNum]->end(); ++itNPC) {
            if ((*itNPC)->Name == m_pNPCTree->GetLastSelect()->text) {
                SNpc* selectedNPC = *(itNPC);
                GetPlayer()->GotoNpcFront(selectedNPC);
                break;
            }
        }
        //itNPC = m_vectorSNpc[m_iSelectMapNum]->begin();
        //SNpc* selectedNPC = *(itNPC + ilistSelect);

        //RtgMatrix16 npcMatrix;
        //npcMatrix.Unit();
        //rtgDecodeUprightDirection(npcMatrix,selectedNPC->Pos[2]);
        //RtCoreLog().Debug("x: %f, y: %f\n",npcMatrix._10,npcMatrix._11);
        //float X = selectedNPC->Pos[0] - npcMatrix._10 * 30;
        //float Y = selectedNPC->Pos[1] - npcMatrix._11 * 30;
        //float Z;
        //g_pScene->GetTerrainHeight(X,Y,Z);
        //GetWorld()->m_pPlayer->AddCommandMoveTo(X+.5f,Y+.5f,Z,-1.f,0.f);

        //RtCoreLog().Debug("npc       x: %d, y: %d\n",selectedNPC->Pos[0]/20,selectedNPC->Pos[1]/20);
        //RtCoreLog().Debug("noc front x: %d, y: %d\n",(int)X/20,(int)Y/20);

        ////float iPosX = selectedNPC->Pos[0]*0.05- 1;
        ////float iPosY = selectedNPC->Pos[1]*0.05- 1;
        ////GetWorld()->m_pPlayer->GotoThere(X/20,Y/20);
    }
    unguard;
}

void CUIForm_MiddleMap::OnShowTaskInfo(ui::RtwWidget* sender, RtwEventDelegate* e) {
    guard;
    if (g_layerMain->m_formTask->IsVisible()) {
    } else {
        g_layerMain->m_formTask->Show();
    }
    g_workspace.SetFocusWidget(LOAD_UI("fmmaintask"));
    LOAD_UI_T(RtwTab, "fmmaintask.fmtask.tabitem")->SetSelectedItem(1);
}

void CUIForm_MiddleMap::SetOnMoveto(int x, int y) {
    guard;
    m_fDesPos[0]     = x * 20;
    m_fDesPos[1]     = y * 20;
    m_bNeedShowPoint = true;

    char POS[8];
    rt2_sprintf(POS, "%d", x);
    m_ptbxPosX->SetText(POS);
    rt2_sprintf(POS, "%d", y);
    m_ptbxPosY->SetText(POS);
    m_isGetPatchPoint = false;
    unguard;
}

void CUIForm_MiddleMap::OnEvSelectList(ui::RtwWidget* sender, RtwEventDelegate* e) {
    guard;
    if (sender == m_pTaskListUI) {
        int ilistSelect = m_pTaskListUI->GetSelectedItem();
        if (ilistSelect == -1) {
            ShowSystemMessage(R(MSG_TASK_SELECTATASKFIRST));
            return;
        }
        string                       taskName = m_pTaskListUI->GetItemText(ilistSelect, 0);
        vector<STaskDesc*>::iterator it;
        STaskDesc*                   desc;
        for (it = m_vectorCanAccept.begin(); it != m_vectorCanAccept.end(); it++) {
            desc = *it;
            if (!desc)
                continue;
            if (desc->name == taskName) {
                m_fDesPos[0]     = desc->m_posTaskBegin[0];
                m_fDesPos[1]     = desc->m_posTaskBegin[1];
                m_bNeedShowPoint = true;
                return;
            }
        }
    } else if (sender == m_pNPCTree) {
        /*int ilistSelect = m_pNPCListUI->GetSelectedItem();
		if(ilistSelect== -1)
		{
			ShowSystemMessage("请先选择一个NPC！");
			return;
		}
		vector<SNpc*>::iterator itNPC;
		itNPC = m_vectorSNpc[m_iSelectMapNum]->begin();
		SNpc* selectedNPC = *(itNPC + ilistSelect);*/
        RtwTree::Item* mRootItem = m_pNPCTree->getRootItem();
        if (mRootItem->pFirstChild == NULL)
            return;
        RtwTree::Item* pSelectItem = m_pNPCTree->GetLastSelect();
        if (pSelectItem == NULL) {
            ShowSystemMessage(R(MSG_SELECTANPCFIRST));
            return;
        }
        vector<SNpc*>::iterator itNPC;
        for (itNPC = m_vectorSNpc[m_iSelectMapNum]->begin();
             itNPC != m_vectorSNpc[m_iSelectMapNum]->end(); ++itNPC) {
            if ((*itNPC)->Name == m_pNPCTree->GetLastSelect()->text) {
                SNpc* selectedNPC = *(itNPC);
                m_fDesPos[0]      = selectedNPC->Pos[0];
                m_fDesPos[1]      = selectedNPC->Pos[1];
                m_bNeedShowPoint  = true;
                break;
            }
        }
    }
    unguard;
}

void CUIForm_MiddleMap::ShowPreview()  //中地图预览显示范围 add by 李泽华
{
    guard;
    //地图部分的绘制
    RtwRect newMapFormRect = m_pfrmMap->getFrameRectRef();
    RtwRect newLabelRect   = RtwRect(newMapFormRect.right + 4, newMapFormRect.top,
                                     newMapFormRect.right + 4 + m_iRealMapWidth / 4,
                                     newMapFormRect.top + m_iRealMapHeight / 4);
    if (m_pPreviewImage && m_previewLabel) {
        g_workspace.AddTopRenderWidget(m_previewLabel);
        m_pPreviewImage->SetBlend(true);
        m_previewLabel->SetBackgroundImage(m_pPreviewImage);
        RtwRect realMapImage = RtwRect(0, 0, m_iRealMapWidth, m_iRealMapHeight);
        m_pPreviewImage->SetSourceRect(realMapImage);
        m_previewLabel->MoveResize(newLabelRect, true);
    }

    //玩家的绘制
    float        fX, fY;
    const float* pPos = &(GetWorld()->m_pPlayer->GetMatrix()->_30);
    fX                = pPos[0];
    fY                = pPos[1];
    g_pScene->OffsetToMap(fX, fY);
    fX     = (fX - m_fRealMapPosX) * 0.032f;
    fY     = m_iRealMapHeight / 4 - (fY - m_fRealMapPosY) * 0.032f;
    int iX = fX;
    int iY = fY;
    iX     = iX + newLabelRect.left;
    iY     = iY + newLabelRect.top;
    m_previewLabel->AddChild(m_lbPlayerpreview);
    RtwRect newLabelRects = RtwRect(iX - 4, iY - 4, iX + 3, iY + 3);
    m_lbPlayerpreview->MoveResize(newLabelRects, true);

    //当前窗体范围
    m_previewLabel->AddChild(m_lbPreviwRect);
    RtwRect newLadRectPreviw = RtwRect(
        newLabelRect.left + m_iOffsetX / 4, newLabelRect.top + m_iOffsetY / 4,
        newLabelRect.left + m_iOffsetX / 4 + 510 / 4, newLabelRect.top + m_iOffsetY / 4 + 382 / 4);
    m_lbPreviwRect->MoveResize(newLadRectPreviw, true);

    //显示，延时隐藏控制
    //static int nowTime, oldTime = timeGetTime();
    //nowTime = timeGetTime();
    if (m_bMouseRDown) {
        //oldTime = nowTime;
        m_previewLabel->Show();
        if (newLabelRect.IsContained(iX, iY))
            m_lbPlayerpreview->Show();
        m_lbPreviwRect->Show();
    } else {

        //if (nowTime - oldTime >= 1000) {
        m_previewLabel->Hide();
        m_lbPlayerpreview->Hide();
        m_lbPreviwRect->Hide();
        //oldTime = nowTime;
        //}
    }

    unguard;
}

void CUIForm_MiddleMap::GetPatchPoint() {
    guard;
    if (GetPlayer())  // 寻路路径绘制
    {
        if (!m_vPatchPoint.empty())  // 优化：使用 empty() 检查容器是否为空
            m_vPatchPoint.clear();
        m_pathNode = GetPlayer()->mBaseActor.GetPathNode();
        if (!m_pathNode)  // 优化：提前返回，减少嵌套
            return;

        int        pointprevious = 0;  // 用于记录前一个路径点的索引
        RtgVertex3 vPos, vPos1;        // 用于存储3D顶点位置
        PatchPoint point;              // 用于存储补丁点信息
        int        count, last;
        // 获取所有路径点并返回最后一个路径点的索引
        RtsPathPath::PPos* theStep = m_pathNode->GetAllPath(last);
        if (!theStep)
            return;
        count = last;  // 路径点总数

        // 定义一个 lambda 函数，用于添加插值路径点
        const auto addPoints = [&](float fx, float fy, float fX, float fY, int k) {
            float stepX = (fX - fx) / k;
            float stepY = (fY - fy) / k;
            for (int i = 1; i <= k; ++i) {
                point.x       = fx + i * stepX;
                point.y       = fy + i * stepY;
                point.beFound = false;
                m_vPatchPoint.push_back(point);
            }
        };
        // 遍历路径点
        for (int ii = 0; ii < count; ++ii) {
            // 获取当前路径点的3D坐标
            g_pScene->GetPosByTerrain(theStep[ii].MinX, theStep[ii].MinY, vPos);
            float fX = vPos.x, fY = vPos.y;          // 获取X、Y坐标
            g_pScene->OffsetToMap(fX, fY);           // 将坐标转换为地图坐标
            int iX = fX * 0.128f, iY = fY * 0.128f;  // 缩放坐标

            if (ii) {  // 如果不是第一个路径点
                // 获取前一个路径点的3D坐标
                g_pScene->GetPosByTerrain(theStep[pointprevious].MinX, theStep[pointprevious].MinY,
                                          vPos1);
                float fx = vPos1.x, fy = vPos1.y;        // 获取前一个点的X、Y坐标
                g_pScene->OffsetToMap(fx, fy);           // 将坐标转换为地图坐标
                int ix = fx * 0.128f, iy = fy * 0.128f;  // 缩放坐标
                // 计算当前点与前一个点之间的距离
                int iRange = sqrt(float((iX - ix) * (iX - ix) + (iY - iy) * (iY - iy)));
                if (iRange >= 10) {  // 如果距离大于等于10
                    int k = iRange / 10;
                    // 添加插值点，根据距离的余数判断是否增加额外的点
                    addPoints(fx, fy, fX, fY, k + (iRange % 10 > 5 ? 1 : 0));
                    pointprevious = ii;
                }
            } else {  // 如果是第一个路径点
                // 获取玩家当前位置
                const float* pPos = &(GetWorld()->m_pPlayer->GetMatrix()->_30);
                float        px = pPos[0], py = pPos[1];
                g_pScene->OffsetToMap(px, py);
                int pX = px * 0.128f, pY = py * 0.128f;

                int iRange = sqrt(float((iX - pX) * (iX - pX) + (iY - pY) * (iY - pY)));
                if (iRange >= 10) {
                    int k = iRange / 10;
                    addPoints(px, py, fX, fY, k + (iRange % 10 > 5 ? 1 : 0));
                }
                // 将第一个路径点作为补丁点添加
                point.x       = fX;
                point.y       = fY;
                point.beFound = false;
                m_vPatchPoint.push_back(point);
            }
        }

        if (count)
            m_isGetPatchPoint = true;
    }
    unguard;
}

bool CUIForm_MiddleMap::PatchPointIsFound(PatchPoint& patchpoint) {
    const float* pPos = &(GetWorld()->m_pPlayer->GetMatrix()->_30);
    float        fPlayerX, fPlayerY;
    fPlayerX = pPos[0];
    fPlayerY = pPos[1];
    g_pScene->OffsetToMap(fPlayerX, fPlayerY);
    if (abs(fPlayerX - patchpoint.x) * 0.128 <= 5 && abs(fPlayerY - patchpoint.y) * 0.128 <= 5)
        return true;
    return false;
}

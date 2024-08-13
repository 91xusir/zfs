
#include "scene/rt_scene.h"
#include "rts_scene_xml_loader.h"
#include "scene/rts_SceneManager.h"
#include "scene/rts_scene_terrain.h"
#include "scene/rts_grass_render.h"
#include "core/rt2_core.h"
#include "../../game_client/preConsole.h"

const float s_fBlockBoundaryWidth   = g_fSceneTerrainTileWidth * 40;
const float s_fBlockFlagShowTime    = 4.f;  // 场景块标记显示总共时间(秒)
const float s_fBlockFlagShowTimeIn  = 1.f;  // 场景块标记显示淡入时间(秒)
const float s_fBlockFlagShowTimeOut = 2.f;  // 场景块标记显示淡出时间(秒)

BOOL RtScene::m_bRenderScene      = TRUE;
BOOL RtScene::m_bRenderSceneWater = TRUE;
#if RTS_DRAW_HELPER
BOOL RtScene::m_bRenderObjectBoundbox = FALSE;
#endif

#define USE_OLD_TO_LOADUNLOADOBJECT

RtScene::RtScene() {
    CM_MEMPROTECTOR(m_pCameraSaveBuffer, 64)
    CM_MEMPROTECTOR(m_szMinimapFile, 256)

    m_fPreLoadBlockDistance  = 1800.f;
    m_fLoadBlockDistance     = 1500.f;
    m_fPreLoadObjectDistance = 1250.f;
    m_fLoadObjectDistance    = 1000.f;
#ifdef SCENE_EDITOR
    m_iMaxBufferBlockCount = 100;
#else
    m_iMaxBufferBlockCount = 4;
#endif
    m_pRenderTile = NULL;
    // m_pRenderMapHead = NULL;
    m_pWaterRenderMapHead = NULL;
    m_iRenderTileCur      = 0;
    m_iRenderTileCount    = 0;
    m_iWaterRenderTileCur = 0;

    m_pTexPatternUV = NULL;

    m_bShowBlockFlag           = TRUE;
    m_bOptionWaterCatoptricSky = TRUE;

    m_lightSet.pNext    = NULL;
    m_lightSet.iEnumCnt = 0;

    m_bReflectSceneObject = TRUE;
    m_bReflectOtherActor  = TRUE;
    m_bHaveReflect        = FALSE;
    //m_pRenderObjHead = NULL;
    m_bRenderMiniMapMode = FALSE;

    m_fLastOnFrameMoveTime = 0.f;
    m_fShowBlockFlagTime   = 0.f;
    //m_iCurrentBlockX = -1;
    //m_iCurrentBlockY = -1;
    m_bAsynchronism = false;

#if RTS_DRAW_HELPER
    m_bDrawHelper = FALSE;
#endif
#if DO_CONSOLE
    m_bEnablePickRayRecord = FALSE;
#endif
    // 环境光
    m_bUseDynamicLight  = FALSE;
    m_iCustomLightCount = 0;
    m_lightSkyColor     = 0xFF808080;
    m_lightTerrainAmbient.Set(0.f);

    // 平行光
    m_lightDirect.eMode = RTG_LIGHT_DIRECTION;
    RtgMatrix16 m16;
    m16.Unit();
    m16.RotateLZ(DegreeToRadian(0.f));
    m16.RotateLX(DegreeToRadian(70.f + 180.f));
    m16.GetRow(2, m_lightDirect.vDirection);
    m_lightDirect.vDirection.Normalize();
    m_lightDirect.vDiffuse.Set(1.f);
    m_lightDirect.vSpecular.Set(1.f);

    m_fMinimapWidth  = g_fSceneBlockWidth * 0.5f;
    m_fMinimapHeight = g_fSceneBlockHeight * 0.5f;
    m_fMinimapRotate = 0.f;

    m_ShaderMinimap.bEnableBlendFunc = true;
    m_ShaderMinimap.eBlendSrcFunc    = RTGBLEND_SRCALPHA;
    m_ShaderMinimap.eBlendDstFunc    = RTGBLEND_INVSRCALPHA;
    //m_ShaderMinimap.bColorKey = true;
    m_ShaderMinimap.iNumStage = 2;

    //m_ShaderMinimap.Shaders[1].eColorOp   = RTGTOP_SELECTARG1;
    m_ShaderMinimap.Shaders[1].eColorOp   = RTGTOP_MODULATE;
    m_ShaderMinimap.Shaders[1].eColorArg1 = RTGTA_TEXTURE;
    m_ShaderMinimap.Shaders[1].eColorArg2 = RTGTA_CURRENT;
    m_ShaderMinimap.Shaders[1].eAlphaOp   = RTGTOP_SELECTARG1;
    m_ShaderMinimap.Shaders[1].eAlphaArg1 = RTGTA_TEXTURE;
    //m_ShaderMinimap.Shaders[1].eAlphaArg2 = RTGTA_CURRENT;
    m_ShaderMinimap.Shaders[1].SetTexture(0);

    m_ShaderMinimap.Shaders[0].eColorOp   = RTGTOP_SELECTARG1;
    m_ShaderMinimap.Shaders[0].eColorArg1 = RTGTA_TEXTURE;
    m_ShaderMinimap.Shaders[0].eColorArg2 = RTGTA_CURRENT;
    m_ShaderMinimap.Shaders[0].eAlphaOp   = RTGTOP_DISABLE;
    m_ShaderMinimap.Shaders[0].eAlphaArg1 = RTGTA_CURRENT;

    mfObjectVisibleDistance = 2.0f;
    mfBlendDistance         = 1.0f;
    mkHeroPosition          = RtgVertex3(0.0f, 0.0f, 0.0f);
    m_bEnableFadeInOut      = true;

    m_bHaveReflect       = FALSE;
    m_pFirstDynamicLight = NULL;

    m_ObjectRender.Reset();
    m_SceneRender.Reset();
    m_GrassRender.Reset();

    rt2_strcpy(m_szMinimapFile, "scene\\minimap\\");
    m_pathPoint    = RtsWayPoint::GetInstance();  //tim.yang   初始化RtsWayPoint实例
    m_bSceneModify = false;
}

RtScene::~RtScene() {
    CM_MEMUNPROTECTOR(m_pCameraSaveBuffer)
    CM_MEMUNPROTECTOR(m_szMinimapFile)
}

// 一些变量的初始化，这些初始化只做一次
bool RtScene::OnceInit(RtgDevice* pDevice) {
    m_pDevice = pDevice;
    m_SceneRender.Init();
    m_GrassRender.Init(m_pDevice);
    m_SceneBlockManager.Init();

    const float fXOffset      = 0.125f;
    const float fYOffset      = 0.25f;
    const float fXDeltaOffset = 0.002;
    const float fYDeltaOffset = 0.002;
    int         i;

    // 贴图模版初始化
    m_pTexPatternUV               = RT_NEW RtgVertex2[32 * 4];
    m_pTexPatternUV[16 * 4 + 0].x = m_pTexPatternUV[0].x = (fXOffset - fXDeltaOffset);
    m_pTexPatternUV[16 * 4 + 0].y = m_pTexPatternUV[0].y = 1 - (fYOffset - fYDeltaOffset);
    m_pTexPatternUV[16 * 4 + 1].x = m_pTexPatternUV[1].x = 0.0f + fXDeltaOffset;
    m_pTexPatternUV[16 * 4 + 1].y = m_pTexPatternUV[1].y = 1 - (fYOffset - fYDeltaOffset);
    m_pTexPatternUV[16 * 4 + 2].x = m_pTexPatternUV[2].x = (fXOffset - fXDeltaOffset);
    m_pTexPatternUV[16 * 4 + 2].y = m_pTexPatternUV[2].y = 1 - (0.0f + fYDeltaOffset);
    m_pTexPatternUV[16 * 4 + 3].x = m_pTexPatternUV[3].x = 0.0f + fXDeltaOffset;
    m_pTexPatternUV[16 * 4 + 3].y = m_pTexPatternUV[3].y = 1 - (0.0f + fYDeltaOffset);

    for (i = 1 * 4; i < 4 * 4; i++) {
        m_pTexPatternUV[i].x = m_pTexPatternUV[i - 4].x + fXOffset;
        m_pTexPatternUV[i].y = m_pTexPatternUV[i - 4].y;
    }
    for (i = 4 * 4; i < 16 * 4; i++) {
        m_pTexPatternUV[i].x = m_pTexPatternUV[i - 16].x;
        m_pTexPatternUV[i].y = m_pTexPatternUV[i - 16].y - fYOffset;
    }
    for (i = 16 * 4; i < 32 * 4; i++) {
        m_pTexPatternUV[i].x = m_pTexPatternUV[i - 16 * 4].x + fXOffset * 4;
        m_pTexPatternUV[i].y = m_pTexPatternUV[i - 16 * 4].y;
    }
    // C for terrain mirror problem [4/17/2009 Louis.Huang]
    // 	for(i=0;i<32*4;i++)
    // 	{
    // 		m_pTexPatternUV[i].x = 1-m_pTexPatternUV[i].x;
    // 		m_pTexPatternUV[i].y = 1-m_pTexPatternUV[i].y;
    // 	}

    for (i = 0; i < 32; i++) {
        m_TexUVPattern[i].iCnt = 1;
        m_TexUVPattern[i].pUV  = m_pTexPatternUV + i * 4;
    }

    m_iRenderTileCount = 1000;
    m_pRenderTile      = RT_NEW RtsSRenderTile[m_iRenderTileCount];

    // 贴图文件名初始化
    m_texMaps[0].iType  = 0;
    m_texMaps[0].szFile = "terrain/terrain_unknow.tga";  // 设置默认值为unknow
    for (i = 1; i < MAX_MAP_COUNT; i++) {
        m_texMaps[i].szFile  = m_texMaps[0].szFile;  // 设置默认值为unknow
        m_texMaps[i].iType   = m_texMaps[0].iType;
        m_texMaps[i].texItem = NULL;
    }

    m_FootPrint.InitOnce(m_pDevice, 30);

    OptionSetWaterCatoptricSky(TRUE);

    CXmlFileMapTexture xmlMapTexture(this);
    xmlMapTexture.ParserFile("scene/other/scene_maptexlist.xml");

    CXmlFileWater xmlWater(this);
    xmlWater.ParserFile("scene/other/scene_water.xml");

    CXmlFileLightSet xmlLightSet(this);
    xmlLightSet.ParserFile("scene/lights/light_default.xml");

    m_GrassRender.LoadGrassTexture("scene/other/scene_grass_texlist.xml");

#ifndef USE_OLD_TO_LOADUNLOADOBJECT
    mpkSceneManager = RT_NEW RtSceneManager();
#endif

    return true;
}

bool RtScene::OnceClear() {
#ifndef USE_OLD_TO_LOADUNLOADOBJECT
    DEL_ONE(mpkSceneManager);
#endif
    m_FootPrint.ClearOnce();

    if (m_ShaderMinimap.Shaders[1].GetTexture())
        RtGetRender()->GetTextMgr()->ReleaseTexture(m_ShaderMinimap.Shaders[1].GetTexture());

    for (int i = 0; i < MAX_MAP_COUNT; ++i)
        if (m_texMaps[i].texItem)
            RtGetRender()->GetTextMgr()->ReleaseTexture(m_texMaps[i].texItem);

    for (int i = 0; i < MAX_WATER_COUNT; i++)
        if (m_WaterRenderMap[i].bLoaded)
            m_pDevice->DeleteShader(&(m_WaterRenderMap[i].Shader));

    if (m_shadowBlockFlag.Shaders[0].GetTexture())
        m_pDevice->DeleteShader(&m_shadowBlockFlag);

    DEL_ARRAY(m_pRenderTile);
    DEL_ARRAY(m_pTexPatternUV);

    RtsSLightSet *pDel, *pEnum;
    pEnum = m_lightSet.pNext;

    while (pEnum) {
        pDel  = pEnum;
        pEnum = pEnum->pNext;
        DEL_ONE(pDel);
    }

    return true;
}

bool RtScene::Init() {
    InitMap();
    Clear();
    return true;
}

void RtScene::ResetRenderEntity() {
    m_ObjectRender.Reset();
    m_GrassRender.Reset();
    m_SceneRender.Reset();
    m_listRenderGrid.clear();
    m_vecTransAct.clear();
    m_iRenderTileCur      = 0;
    m_pWaterRenderMapHead = NULL;
    m_iWaterRenderTileCur = 0;
}

void RtScene::Clear() {
    ResetRenderEntity();
    ClearMap();
    RtObjectManager()->Clear();
    m_lightSkyColor = 0;
}

void RtScene::OptionSetWaterCatoptricSky(BOOL bTrue) {
    m_bOptionWaterCatoptricSky = bTrue;
    for (int i = 0; i < MAX_WATER_COUNT; i++) {
        if (m_WaterRenderMap[i].bLoaded) {
            SetupWaterShader(i + 1);
        }
    }
}

void RtScene::SetupWaterShader(BYTE cWaterType) {
    int iIdx = cWaterType - 1;
    if (iIdx < 0 || iIdx >= MAX_WATER_COUNT)
        return;
    if (m_WaterRenderMap[iIdx].bLoaded) {
        m_pDevice->DeleteShader(&(m_WaterRenderMap[iIdx].Shader));
    }

    m_WaterRenderMap[iIdx].bLoaded                 = true;
    m_WaterRenderMap[iIdx].dwColor                 = m_Waters[iIdx].dwColor;
    m_WaterRenderMap[iIdx].fColorRate              = m_Waters[iIdx].fColorRate;
    m_WaterRenderMap[iIdx].Shader.bEnableBlendFunc = true;
    m_WaterRenderMap[iIdx].Shader.iNumStage        = 1;
    m_WaterRenderMap[iIdx].Shader.Shaders[0].SetTexture(
        RtGetRender()->GetTextMgr()->CreateTextureFromFile(m_Waters[iIdx].szFile));
    m_WaterRenderMap[iIdx].Shader.Shaders[0].eAlphaArg1 = RTGTA_DIFFUSE;
    m_WaterRenderMap[iIdx].Shader.Shaders[0].mMatrix16.Unit();
    if (m_bOptionWaterCatoptricSky) {
        m_WaterRenderMap[iIdx].Shader.iNumStage       = 2;
        m_WaterRenderMap[iIdx].Shader.dwTextureFactor = 0x10FFFFFF;  // Alpha值
        m_WaterRenderMap[iIdx].Shader.Shaders[1].SetTexture(
            RtGetRender()->GetTextMgr()->CreateTextureFromFile("sky_001.tga"));
        m_WaterRenderMap[iIdx].Shader.Shaders[1].mMatrix16.Unit();
        m_WaterRenderMap[iIdx].Shader.Shaders[1].eTexCoordIndex = RTGTC_CAMERASPACEREFLECTIONVECTOR;
        m_WaterRenderMap[iIdx].Shader.Shaders[1].eColorOp       = RTGTOP_BLENDFACTORALPHA;
        m_WaterRenderMap[iIdx].Shader.Shaders[1].eColorArg2     = RTGTA_CURRENT;
        m_WaterRenderMap[iIdx].Shader.Shaders[1].eAlphaArg1     = RTGTA_CURRENT;
    }
}

void RtScene::ResetTimer() {}

void RtScene::OnFrameMove(float _elapsedTime) {
    m_SceneBlockManager.FrameMove();
    PreRender(*RtGetRender()->m_pCamera);
    m_ObjectRender.Update(_elapsedTime);
    m_FootPrint.Run(_elapsedTime);

    if (m_bShowBlockFlag)
        if (m_fShowBlockFlagTime > 0.f)
            m_fShowBlockFlagTime -= _elapsedTime;
}

void RtScene::RequestBlockReady(const RtgVertex3& _Pos, bool bForcePreLoad /* = false */) {
    if (m_iBlockXCnt == 0 || m_iBlockYCnt == 0)
        return;

    RtsSceneBlockMap* blockMap = NULL;
    RtgVertex4        _range;
    int               _Indexx = 0;
    int               _Indexy = 0;
    Int2              _plIndex[8];
    bool              _plForce[8];
    int               _plIndexNum = 0;

    GetBlockInfo(RtgVertex2(_Pos.x, _Pos.y), _Indexx, _Indexy, _plIndex, _plForce, _plIndexNum);

    if ((blockMap = GetBlockMapbyIndex(_Indexx, _Indexy)) && blockMap->Check_CanLoad()) {
        begin_log_tick(0);
        m_SceneBlockManager.CreateSceneBlock(blockMap, m_szFilePath, _Indexx - m_iCenterPosX,
                                             _Indexy - m_iCenterPosY, true);
        end_log_tick2(0, "force load scene block", blockMap->m_szFileName);
    }

    for (size_t i = 0; i < _plIndexNum; ++i) {
        if ((blockMap = GetBlockMapbyIndex(_plIndex[i].x, _plIndex[i].y)) &&
            blockMap->Check_CanLoad()) {
            m_SceneBlockManager.CreateSceneBlock(blockMap, m_szFilePath,
                                                 _plIndex[i].x - m_iCenterPosX,
                                                 _plIndex[i].y - m_iCenterPosY, bForcePreLoad);
            RtCoreLog().Info("preloading scene block : %s\n", blockMap->m_szFileName);
        }
    }

    RequestBlockObject(_Pos);

    if (!m_bRenderMiniMapMode)
        RequestEnvUpdate(_Pos);

#if !defined(_DLL)
    m_SceneBlockManager.EvictResources(Int2(_Indexx - m_iCenterPosX, _Indexy - m_iCenterPosY));
#endif
}

void RtScene::RequestBlockObject(const RtgVertex3& _Pos) {
    int _gridMinx = 0;
    int _gridMaxx = 0;
    int _gridMiny = 0;
    int _gridMaxy = 0;

    GetGridByPos(_Pos.x - m_fLoadObjectDistance, _Pos.y - m_fLoadObjectDistance, _gridMinx,
                 _gridMiny);
    GetGridByPos(_Pos.x + m_fLoadObjectDistance, _Pos.y + m_fLoadObjectDistance, _gridMaxx,
                 _gridMaxy);

    for (int y = _gridMiny; y <= _gridMaxy; ++y) {
        for (int x = _gridMinx; x <= _gridMaxx; ++x) {
            RtsSGrid* _grid = GetGrid(x, y, false);

            if (_grid && !_grid->bObjLoad) {
                for (int i = 0; i < _grid->aryObject.size(); ++i) {
                    RtsObject* _obj = RtObjectManager()->GetObject(_grid->aryObject[i]);

                    if (_obj && _obj->GetState() == Object_State_New)
                        _obj->RecreateResource(_grid);
                }

                _grid->bObjLoad = true;
            }
        }
    }
}

void RtScene::RequestEnvUpdate(const RtgVertex3& _Pos) {
    int               _blockx   = 0;
    int               _blocky   = 0;
    RtsSceneBlockMap* _blockMap = GetBlockMapbyPos(RtgVertex2(_Pos.x, _Pos.y), _blockx, _blocky);

    if (!_blockMap || _blockMap->GetState() != Object_State_Ready)
        return;

    RtSceneBlock* _block = (RtSceneBlock*)_blockMap->m_pBlock;
    RTASSERT(_block);

    if (!_block)
        return;

    RtsSceneBlockMap* _horBlock = NULL;
    RtsSceneBlockMap* _velBlock = NULL;
    RtsSceneBlockMap* _adjBlock = NULL;
    bool              bleft     = false;
    bool              brigt     = false;
    bool              btop      = false;
    bool              bbottom   = false;
    float             r1        = 0.f;
    float             r2        = 0.f;

    if ((_Pos.x - _block->m_vMin.x) < s_fBlockBoundaryWidth) {
        if (_horBlock = GetBlockMapbyIndex(_blockx - 1, _blocky))
            r1 = (s_fBlockBoundaryWidth - (_Pos.x - _block->m_vMin.x)) /
                 (s_fBlockBoundaryWidth * 2.f);
        bleft = true;
    } else if ((_block->m_vMax.x - _Pos.x) < s_fBlockBoundaryWidth) {
        if (_horBlock = GetBlockMapbyIndex(_blockx + 1, _blocky))
            r1 = (s_fBlockBoundaryWidth - (_block->m_vMax.x - _Pos.x)) /
                 (s_fBlockBoundaryWidth * 2.f);
        bleft = false;
    }

    if ((_Pos.y - _block->m_vMin.y) < s_fBlockBoundaryWidth) {
        if (_velBlock = GetBlockMapbyIndex(_blockx, _blocky - 1))
            r2 = (s_fBlockBoundaryWidth - (_Pos.y - _block->m_vMin.y)) /
                 (s_fBlockBoundaryWidth * 2.f);
        btop = true;
    } else if ((_block->m_vMax.y - _Pos.y) < s_fBlockBoundaryWidth) {
        if (_velBlock = GetBlockMapbyIndex(_blockx, _blocky + 1))
            r2 = (s_fBlockBoundaryWidth - (_block->m_vMax.y - _Pos.y)) /
                 (s_fBlockBoundaryWidth * 2.f);
        btop = false;
    }

    if (_horBlock && _velBlock) {
        if (bleft) {
            if (btop)
                _adjBlock = GetBlockMapbyIndex(_blockx - 1, _blocky - 1);
            else
                _adjBlock = GetBlockMapbyIndex(_blockx - 1, _blocky + 1);
        } else {
            if (btop)
                _adjBlock = GetBlockMapbyIndex(_blockx + 1, _blocky - 1);
            else
                _adjBlock = GetBlockMapbyIndex(_blockx + 1, _blocky + 1);
        }
    }

    RtgVertex3 color0;
    RtgVertex3 color1;
    RtgVertex3 color2;

    if (_horBlock)
        color0 = RtgColorToVector(_horBlock->m_dwEnvironmentLighting);
    else
        color0.Set(0.f);
    if (_velBlock)
        color1 = RtgColorToVector(_velBlock->m_dwEnvironmentLighting);
    else
        color1.Set(0.f);
    if (_adjBlock)
        color2 = RtgColorToVector(_adjBlock->m_dwEnvironmentLighting);
    else
        color2.Set(0.f);

    color0 = RtgColorToVector(_blockMap->m_dwEnvironmentLighting) * (1.f - r1) + color0 * r1;
    color1 = color1 * (1.f - r1) + color2 * r1;
    color2 = color0 * (1.f - r1) + color1 * r2;
    SetSkyLighting(RtgVectorToColor(color2, 1.f));

    if (_horBlock)
        color0 = RtgColorToVector(_horBlock->m_dwSunLighting);
    else
        color0.Set(0.f);
    if (_velBlock)
        color1 = RtgColorToVector(_velBlock->m_dwSunLighting);
    else
        color1.Set(0.f);
    if (_adjBlock)
        color2 = RtgColorToVector(_adjBlock->m_dwSunLighting);
    else
        color2.Set(0.f);

    color0 = RtgColorToVector(_blockMap->m_dwSunLighting) * (1.f - r1) + color0 * r1;
    color1 = color1 * (1.f - r1) + color2 * r1;
    color2 = color0 * (1.f - r2) + color1 * r2;
    m_lightDirect.vDiffuse = color2;

    if (_horBlock)
        color0 = RtgColorToVector(_horBlock->m_dwFogColor);
    else
        color0.Set(0.f);
    if (_velBlock)
        color1 = RtgColorToVector(_velBlock->m_dwFogColor);
    else
        color1.Set(0.f);
    if (_adjBlock)
        color2 = RtgColorToVector(_adjBlock->m_dwFogColor);
    else
        color2.Set(0.f);
    color0           = RtgColorToVector(_blockMap->m_dwFogColor) * (1.f - r1) + color0 * r1;
    color1           = color1 * (1.f - r1) + color2 * r1;
    color2           = color0 * (1.f - r2) + color1 * r2;
    DWORD dwFogColor = RtgVectorToColor(color2, 1.f);
    RtGetRender()->SetFogColor(dwFogColor);

    float f0 = 0.f;
    float f1 = 0.f;
    float f2 = 0.f;

    if (_horBlock)
        f0 = _horBlock->m_fFogNear;
    else
        f0 = 100.f;
    if (_velBlock)
        f1 = _velBlock->m_fFogNear;
    else
        f1 = 100.f;
    if (_adjBlock)
        f2 = _adjBlock->m_fFogNear;
    else
        f2 = 100.f;
    f0 = _blockMap->m_fFogNear * (1.f - r1) + f0 * r1;
    f1 = f1 * (1.f - r1) + f2 * r1;
    f2 = f0 * (1.f - r2) + f1 * r2;
    RtGetRender()->SetFogNear(f2);

    if (_horBlock)
        f0 = _horBlock->m_fFogFar;
    else
        f0 = 100.f;
    if (_velBlock)
        f1 = _velBlock->m_fFogFar;
    else
        f1 = 100.f;
    if (_adjBlock)
        f2 = _adjBlock->m_fFogFar;
    else
        f2 = 100.f;
    f0 = _blockMap->m_fFogFar * (1.f - r1) + f0 * r1;
    f1 = f1 * (1.f - r1) + f2 * r1;
    f2 = f0 * (1.f - r2) + f1 * r2;
    RtGetRender()->SetFogFar(f2);

    if (_horBlock)
        f0 = _horBlock->m_fPostProcessEffectIntensity;
    else
        f0 = 0.f;
    if (_velBlock)
        f1 = _velBlock->m_fPostProcessEffectIntensity;
    else
        f1 = 0.f;
    if (_adjBlock)
        f2 = _adjBlock->m_fPostProcessEffectIntensity;
    else
        f2 = 0.f;
    f0 = _blockMap->m_fPostProcessEffectIntensity * (1.f - r1) + f0 * r2;
    f1 = f1 * (1.f - r1) + f2 * r1;
    f2 = f0 * (1.f - r2) + f1 * r2;
    RtGetRender()->m_fPostProcessEffectIntensity = f2;

    if (_horBlock)
        color0 = _horBlock->m_sunLightDir;
    else
        color0.Set(0.f);
    if (_velBlock)
        color1 = _velBlock->m_sunLightDir;
    else
        color1.Set(0.f);
    if (_adjBlock)
        color2 = _adjBlock->m_sunLightDir;
    else
        color2.Set(0.f);
    color0                   = RtgVertex3(_blockMap->m_sunLightDir) * (1.f - r1) + color0 * r1;
    color1                   = color1 * (1.f - r1) + color2 * r1;
    m_lightDirect.vDirection = color0 * (1.f - r2) + color1 * r2;
    m_lightDirect.vDirection.Normalize();
}

void RtScene::RequestBlockReadyForSM(const Int2& _blockIndex) {
    RtsSceneBlockMap* _blockMap = GetBlockMapbyIndex(_blockIndex.x, _blockIndex.y);

    if (!_blockMap)
        return;

    m_SceneBlockManager.EvictResources(Int2(_blockMap->m_blockPosx, _blockMap->m_blockPosy));

    int _minx = max(_blockIndex.x - 1, 0);
    int _miny = max(_blockIndex.y - 1, 0);
    int _maxx = min(_blockIndex.x + 1, GetBlockCntX());
    int _maxy = min(_blockIndex.y + 1, GetBlockCntY());

    for (int y = _miny; y < _maxy; ++y) {
        for (int x = _minx; x < _maxx; ++x) {
            RtsSceneBlockMap* _bmap = GetBlockMapbyIndex(x, y);

            if (_bmap && _bmap->Check_CanLoad()) {
                m_SceneBlockManager.CreateSceneBlock(_bmap, m_szFilePath, x - GetCenterX(),
                                                     y - GetCenterY(), true);
            }

            if (_bmap->GetState() == Object_State_Ready)
                RequestBlockObjectForSM(Int2(x, y));
        }
    }
}

void RtScene::RequestBlockObjectForSM(const Int2& _blockIndex) {
    RtsSceneBlockMap* _blockMap = GetBlockMapbyIndex(_blockIndex.x, _blockIndex.y);
    RTASSERT(_blockMap);
    RTASSERT(_blockMap->GetState() == Object_State_Ready);

    RtSceneBlock* _block = (RtSceneBlock*)_blockMap->m_pBlock;

    for (int i = 0; i < _block->m_iGridSize; ++i) {
        RtsSGrid* _grid = _block->m_pGrid + i;

        if (_grid && !_grid->bObjLoad) {
            for (int j = 0; j < _grid->aryObject.size(); ++j) {
                RtsObject* _obj = RtObjectManager()->GetObject(_grid->aryObject[j]);

                if (!_obj)
                    continue;

                if (_obj && _obj->GetState() == Object_State_New)
                    _obj->RecreateResource(_grid, true);
                _obj->FrameMove(2800.f);
            }

            _grid->bObjLoad = true;
        }
    }
}

void RtScene::OnBlockLoadFinish(RtsSceneBlockMap* _blockMap) {
#ifdef RT_SCENE_ONLY_LOAD_TERRAIN
    return;
#endif

    RTASSERT(_blockMap && _blockMap->m_pBlock);
    RtSceneBlock* _block = (RtSceneBlock*)_blockMap->m_pBlock;

    _block->UpdateGrid(0, 0, g_iSceneTileCntX, g_iSceneTileCntY);
    _block->OnChangeBorderline(true);
    _block->SetModifyFlag(false);

    for (int i = 0; i < _block->m_iGridSize; ++i) {
        RtsSGrid* pGrid = _block->m_pGrid + i;

        for (int y = 0; y < RTS_BLOCK_GRID_SIZE; ++y) {
            for (int x = 0; x < RTS_BLOCK_GRID_SIZE; ++x) {
                int          tileId  = ((y + pGrid->y) * g_iSceneTileCntX + (x + pGrid->x));
                RtsSTileMap* tileMap = pGrid->pBlock->m_pTileMapIdx + tileId;

                RTASSERT(tileMap->mapCnt <= 4);

                for (int j = 0; j < tileMap->mapCnt; ++j) {
                    int rmId = ((tileMap->dwSortMapIdx >> (j << 3)) & 0x0ff);

                    if (!m_texMaps[rmId].texItem) {
                        m_texMaps[rmId].texItem =
                            RtGetRender()->GetTextMgr()->CreateTextureFromFile(
                                m_texMaps[rmId].szFile, D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT,
                                D3DFMT_FROM_FILE, D3DPOOL_MANAGED, 0);
                        if (!m_texMaps[rmId].texItem) {
                            m_texMaps[rmId].texItem =
                                RtGetRender()->GetTextMgr()->GetChessboardText();
                            m_texMaps[rmId].texItem->Ref++;
                        }
                    }
                }
            }
        }
    }

    m_bSceneModify = true;
}

void RtScene::ShowSceneFlagTexture(const char* szFlagTexture, bool bSaveLastTexture /* = true*/) {
    RtCoreLog().Info("ShowSceneFlagTexture [%s]\n", szFlagTexture);

    if (szFlagTexture == NULL) {
        m_strShowBlockFlagTextureName = "";
        return;
    }

    if (strcmp(m_strShowBlockFlagTextureName, szFlagTexture) == 0)
        return;

    if (bSaveLastTexture)
        m_strShowBlockFlagTextureName = szFlagTexture;

    RtgTextItem* texItem = RtGetRender()->GetTextMgr()->CreateTextureFromFile(szFlagTexture);

    if (texItem) {
        if (m_shadowBlockFlag.Shaders[0].GetTexture())
            m_pDevice->DeleteShader(&m_shadowBlockFlag);

        m_shadowBlockFlag.dwTextureFactor       = 0x00FFFFFF;
        m_shadowBlockFlag.iNumStage             = 1;
        m_shadowBlockFlag.bEnableBlendFunc      = TRUE;
        m_shadowBlockFlag.eBlendSrcFunc         = RTGBLEND_SRCALPHA;
        m_shadowBlockFlag.eBlendDstFunc         = RTGBLEND_INVSRCALPHA;
        m_shadowBlockFlag.Shaders[0].eColorOp   = RTGTOP_MODULATE;
        m_shadowBlockFlag.Shaders[0].eAlphaOp   = RTGTOP_MODULATE;
        m_shadowBlockFlag.Shaders[0].eAlphaArg2 = RTGTA_TFACTOR;
        m_shadowBlockFlag.Shaders[0].SetTexture(texItem);

        m_fShowBlockFlagTime = s_fBlockFlagShowTime;
    }
}

bool RtScene::LoadMap(const char* szMapFileName, bool bLoadAll) {
    return RtsSceneDoc::LoadMap(szMapFileName, bLoadAll);
}

bool RtScene::OnSerializeObjects(RtArchive* pFile) {
    if (pFile->IsLoading()) {
        size_t _objNum = 0;
        *pFile << _objNum;
    } else {
        size_t _objNum = RtObjectManager()->ObjectNum();
        *pFile << _objNum;
    }

    return true;
}

void RtScene::OnModelCreateFinish(RtsModel* _model, RtsSGrid* _grid) {
    if (!_model || !_grid)
        return;

    float _h = _model->Get_World_AABBox()->Max().z;

    if (_h > _grid->vMax.z)
        _grid->vMax.z = _h;

    if (_h > _grid->pBlock->m_vMax.z)
        _grid->pBlock->m_vMax.z = _h;
}

void SGetTerrainVertex(RtSceneBlock* pScene, int iIdx, int x, int y, RtgVertex3& v) {
    RtsSVertex* pV        = pScene->m_pVertices + (y * g_iSceneTileCntX + x) * 4;
    bool        bTwoValue = true;
    RtgVertex3  v1, v2;
    switch (iIdx) {
        case 0:
            v1        = pV[0].pPos->v;
            v1.z      = pV[0].pPos->fTerrainHeight;
            bTwoValue = false;
            break;
        case 1:
            v1   = pV[0].pPos->v;
            v1.z = pV[0].pPos->fTerrainHeight;
            v2   = pV[1].pPos->v;
            v2.z = pV[1].pPos->fTerrainHeight;
            break;
        case 2:
            v1        = pV[1].pPos->v;
            v1.z      = pV[1].pPos->fTerrainHeight;
            bTwoValue = false;
            break;
        case 3:
            v1   = pV[0].pPos->v;
            v1.z = pV[0].pPos->fTerrainHeight;
            v2   = pV[2].pPos->v;
            v2.z = pV[2].pPos->fTerrainHeight;
            break;
        case 4:
            v1   = pV[1].pPos->v;
            v1.z = pV[1].pPos->fTerrainHeight;
            v2   = pV[2].pPos->v;
            v2.z = pV[2].pPos->fTerrainHeight;
            break;
        case 5:
            v1   = pV[1].pPos->v;
            v1.z = pV[1].pPos->fTerrainHeight;
            v2   = pV[3].pPos->v;
            v2.z = pV[3].pPos->fTerrainHeight;
            break;
        case 6:
            v1        = pV[2].pPos->v;
            v1.z      = pV[2].pPos->fTerrainHeight;
            bTwoValue = false;
            break;
        case 7:
            v1   = pV[2].pPos->v;
            v1.z = pV[2].pPos->fTerrainHeight;
            v2   = pV[3].pPos->v;
            v2.z = pV[3].pPos->fTerrainHeight;
            break;
        case 8:
            v1        = pV[3].pPos->v;
            v1.z      = pV[3].pPos->fTerrainHeight;
            bTwoValue = false;
            break;
    }
    if (bTwoValue) {
        v = (v1 + v2) / 2.f;
    } else {
        v = v1;
    }
}

void RtScene::DrawTerrain(RtgCamera& inCamera, BYTE cAttrMask, DWORD dwNormalColor,
                          DWORD dwHitColor, float fOffset) {
    int x, y, i, j;

    DWORD        dwVertexCount;
    RtgShader    terrainShader;
    RtgShader    terrainLineShader;
    RtgVertexVC  pVertices[RTS_BLOCK_GRID_SIZE * RTS_BLOCK_GRID_SIZE * 24];
    RtgVertexVC* pV;
    RtgVertex3   v[9];
    DWORD        dwColor;
    int iTriangleIdx[24] = {0, 1, 3, 1, 4, 3, 1, 2, 4, 2, 5, 4, 3, 4, 6, 4, 7, 6, 4, 5, 7, 5, 8, 7};
    CHECK(g_iSceneTileRate == 2);  // 必须为2，否则这个函数的内容要改

    terrainShader.dwTextureFactor       = 0x80FFFFFF;
    terrainShader.iNumStage             = 1;
    terrainShader.bEnableBlendFunc      = true;
    terrainShader.eBlendSrcFunc         = RTGBLEND_SRCALPHA;
    terrainShader.eBlendDstFunc         = RTGBLEND_INVSRCALPHA;
    terrainShader.Shaders[0].eColorOp   = RTGTOP_SELECTARG1;
    terrainShader.Shaders[0].eColorArg1 = RTGTA_DIFFUSE;
    terrainShader.Shaders[0].eAlphaOp   = RTGTOP_SELECTARG1;
    terrainShader.Shaders[0].eAlphaArg1 = RTGTA_TFACTOR;

    terrainLineShader.iNumStage = 0;

    RtgMatrix16 m16;
    m16.Unit();
    m_pDevice->SetRenderState(RTGRS_LIGHTING, FALSE);
    m_pDevice->SetMatrix(RTGTS_WORLD, &m16);
    m_pDevice->SetRenderState(RTGRS_Z_WRITE, FALSE);
    m_pDevice->SetVertexFormat(RTG_VC);

    for (gridlist_t::iterator it = m_listRenderGrid.begin(); it != m_listRenderGrid.end(); ++it) {
        RtsSGrid* pGrid = (*it);

        // 开始组织三角形
        dwVertexCount = 0;
        pV            = pVertices;
        for (y = 0; y < RTS_BLOCK_GRID_SIZE; y++) {
            for (x = 0; x < RTS_BLOCK_GRID_SIZE; x++) {
                for (i = 0; i < 24; i++) {
                    SGetTerrainVertex(pGrid->pBlock, iTriangleIdx[i], x + pGrid->x, y + pGrid->y,
                                      pV[i].v);
                    pV[i].v.z += fOffset;
                }
                for (i = 0; i < 4; i++) {
                    //jin.wang
                    dwColor = dwNormalColor;
                    BYTE b  = pGrid->pBlock->GetTerrainAttr((x + pGrid->x) * 2 + i % 2,
                                                            (y + pGrid->y) * 2 + i / 2);

                    //判断 当获取的格子属性第四位为1，则说明该点为路点，并将颜色置为蓝色	ac
                    if (b & 0x8) {
                        dwColor = 0xff0000ff;
                    }
                    if (b & cAttrMask) {
                        if (b & 0x01)
                            dwColor = dwHitColor;
                        else if (b & 0x02)
                            dwColor = 0xff0000ff;
                    }
                    if (cAttrMask & 0x8 && dwColor == dwNormalColor) {
                        if (!pGrid->pBlock->CanWalk((x + pGrid->x) * 2 + i % 2,
                                                    (y + pGrid->y) * 2 + i / 2)) {
                            dwColor = 0xff0000ff;
                        }
                    }
                    //jin.wang disabled
                    //tim.yang 判断是否是路点列表中的点，并且当前点不是阻挡区域，满足条件的话绘制成蓝色
                    //POINT p;
                    ////jin.wang
                    //               p.x = (x+pGrid->x)*2+i%2;
                    //p.y = (y+pGrid->y)*2+i/2;
                    //RtSceneBlockTerrain::STileAttr* pTile = pGrid->pBlock->GetTerrain(p.x, p.y);
                    //if(pTile == NULL)
                    //	continue;
                    //if(!pTile->bWalk && m_pathPoint->CheckPathPoint(pTile,g_pScene))		//用了新的判断是否为路点的方法，故原方法舍弃   ac
                    //{
                    //	dwColor = 0xff0000ff;
                    //}
                    ////end
                    for (j = 0; j < 6; j++) {
                        pV->c = dwColor;
                        pV++;
                    }
                }
                dwVertexCount += 24;
                m_pDevice->SetRenderState(RTGRS_Z_WRITE, FALSE);
            }
        }
        // 开始画图
        m_pDevice->SetShader(&terrainShader);
        //m_pDevice->SetTextureAddress(RTGTADD_BORDER);
        /*m_pDevice->SetTextureFilterMin(RTGTEXF_LINEAR);
        m_pDevice->SetTextureFilterMag(RTGTEXF_LINEAR);
        m_pDevice->SetTextureFilterMip(RTGTEXF_LINEAR);*/
        m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, pVertices, sizeof(RtgVertexVC),
                                   dwVertexCount / 3);
        m_pDevice->RestoreShader(&terrainShader);

        m_pDevice->SetRenderState(RTGRS_FILL_MODE, RTG_FILLWIREFRAME);
        m_pDevice->SetShader(&terrainLineShader);
        m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, pVertices, sizeof(RtgVertexVC),
                                   dwVertexCount / 3);
        m_pDevice->RestoreShader(&terrainLineShader);
        m_pDevice->SetRenderState(RTGRS_FILL_MODE, RTG_FILLSOLID);
    }
    m_pDevice->SetRenderState(RTGRS_Z_WRITE, TRUE);
    //m_pDevice->SetRenderState(RTGRS_Z_TEST, TRUE);
}

//
// function : Piexl Access
// author   : zxy
//
namespace {

template <typename pixel_t>
struct _PiexlAccess {
    _PiexlAccess(void* _data, int _width, int _height, int _pitch)
        : pdata(_data), width(_width), height(_height), pitch(_pitch) {}

    inline void set(int x, int y, pixel_t val) {
        if (x < 0 || x >= width || y < 0 || y >= height)
            return;
        ((pixel_t*)(((char*)pdata) + y * pitch))[x] = val;
    }

    inline pixel_t get(int x, int y) {
        if (x < 0 || x >= width || y < 0 || y >= height)
            RTASSERT(false);
        return ((pixel_t*)(((char*)pdata) + y * pitch))[x];
    }

    inline bool get(int x, int y, pixel_t* p) {
        if (x < 0 || x >= width || y < 0 || y >= height)
            return false;
        (*p) = ((pixel_t*)(((char*)pdata) + y * pitch))[x];
        return true;
    }

    inline DWORD get(int sx, int sy, int dx, int dy) {
        RTASSERT(sizeof(DWORD) == sizeof(pixel_t));
        DWORD a    = 0;
        DWORD r    = 0;
        DWORD g    = 0;
        DWORD b    = 0;
        DWORD cNum = 0;
        for (int x = sx; x < dx; ++x)
            for (int y = sy; y < dy; ++y) {
                DWORD c = 0;
                if (get(x, y, &c)) {
                    a += ((c >> 24) & 0xff);
                    r += ((c >> 16) & 0xff);
                    g += ((c >> 8) & 0xff);
                    b += ((c >> 0) & 0xff);
                    ++cNum;
                }
            }
        if (cNum == 0)
            return 0;
        return (DWORD)(((a / cNum) << 24) | ((r / cNum) << 16) | ((g / cNum) << 8) | (b / cNum));
    }

    void cubeBlur(int sizCube = 1) {
        for (int y = 0; y < height; ++y)
            for (int x = 0; x < width; ++x) {
                DWORD c    = 0;
                DWORD cNum = 0;
                for (int i = x - sizCube; i <= x + sizCube; ++i)
                    for (int j = y - sizCube; j <= y + sizCube; ++j) {
                        BYTE t;
                        if (get(i, j, &t)) {
                            c += t;
                            ++cNum;
                        }
                    }
                set(x, y, c / cNum);
            }
    }

    const int width;
    const int height;
    const int pitch;
    void*     pdata;
};
}  // namespace

//
// function : process shadow map
// author   : zxy
//
void RtScene::ProcessSM(RtgTextItem* texSrc, int sizSrc, RtgTextItem* texDst, int sizDst) {
    RTASSERT(sizSrc >= sizDst);

    RtgTextLock lockSrc;
    RtgTextLock lockDst;

    lockSrc.flag = Text_Lock_Read;
    lockDst.flag = Text_Lock_Write;

    if (RtGetRender()->GetTextMgr()->Lock(&lockSrc, texSrc)) {
        if (RtGetRender()->GetTextMgr()->Lock(&lockDst, texDst)) {
            _PiexlAccess<DWORD> ps =
                _PiexlAccess<DWORD>(lockSrc.data, lockSrc.width, lockSrc.height, lockSrc.pitch);
            _PiexlAccess<BYTE> pd =
                _PiexlAccess<BYTE>(lockDst.data, lockDst.width, lockDst.height, lockDst.pitch);

            int wf = ceil((float)lockSrc.width / (float)lockDst.width);
            int hf = ceil((float)lockSrc.height / (float)lockDst.height);

            for (int y = 0; y < lockDst.height; ++y) {
                for (int x = 0; x < lockDst.width; ++x) {
                    if (ps.get(x * wf, y * hf, (x + 1) * wf, (y + 1) * hf) > 0)
                        pd.set(x, y, 140);
                    else
                        pd.set(x, y, 255);
                }
            }

            pd.cubeBlur();
            RtGetRender()->GetTextMgr()->Unlock(&lockDst);
        }

        RtGetRender()->GetTextMgr()->Unlock(&lockSrc);
    }
}

namespace {

struct SM_TexCoordTrans {
    typedef struct _VertexT {
        RtgVertex4 pos;
        RtgVertex2 uv;

        enum { Fvf = RTG_TRANSFORMED_VT };
    } VertexT;

    bool Init(int cellNumX, int cellNumY, int texWidth, int texHeight) {
        float stepX = (float)texWidth / (float)cellNumX;
        float stepY = (float)texHeight / (float)cellNumY;

        int vnum = (cellNumX + 1) * (cellNumY + 1);
        int i    = 0;

        vb = RT_NEW VertexT[cellNumX * cellNumY * 4];
        ib = RT_NEW WORD[cellNumX * cellNumY * 6];

        for (int y = 0; y < cellNumY; ++y) {
            for (int x = 0; x < cellNumX; ++x) {
                vb[i++].pos.Set(x * stepX, texHeight - y * stepY, 0.f, 1.f);
                vb[i++].pos.Set(x * stepX + stepX, texHeight - y * stepY, 0.f, 1.f);
                vb[i++].pos.Set(x * stepX, texHeight - y * stepY - stepY, 0.f, 1.f);
                vb[i++].pos.Set(x * stepX + stepX, texHeight - y * stepY - stepY, 0.f, 1.f);
            }
        }

        WORD* pIb = ib;
        i         = 0;

        while (i < cellNumX * cellNumY) {
            (*pIb++) = i * 4 + 0;
            (*pIb++) = i * 4 + 1;
            (*pIb++) = i * 4 + 2;
            (*pIb++) = i * 4 + 1;
            (*pIb++) = i * 4 + 3;
            (*pIb++) = i * 4 + 2;
            ++i;
        }

        xNum = cellNumX;
        yNum = cellNumY;

        return true;
    }

    void SetUV(int index, float u, float v) { vb[index].uv.Set(u, v); }

    void Render(RtgDevice* _dev, RtgTextItem* texItem) {
        RtgDeviceD3D9* dev = RtGetRender();
        RTASSERT(dev);

        RtgShader sh;
        sh.iNumStage = 1;
        sh.Shaders[0].SetTexture(texItem);

        RtgShareBufferItem vbItem;
        RtgShareBufferItem ibItem;
        BYTE*              pvb =
            (BYTE*)dev->GetBuffMgr()->Lock_Shared_vb(xNum * yNum * 4 * sizeof(VertexT), &vbItem);
        WORD* pib =
            (WORD*)dev->GetBuffMgr()->Lock_Shared_ib(xNum * yNum * 6 * sizeof(WORD), &ibItem);
        memcpy(pvb, vb, xNum * yNum * 4 * sizeof(VertexT));
        memcpy(pib, ib, xNum * yNum * 6 * sizeof(WORD));
        dev->GetBuffMgr()->Unlock(&vbItem);
        dev->GetBuffMgr()->Unlock(&ibItem);
        dev->SetVertexFormat((RTGVertexFormat)VertexT::Fvf);
        dev->SetShader(&sh);
        dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, &vbItem, sizeof(VertexT), &ibItem,
                                  xNum * yNum * 2);
    }

    void Clear() {
        if (vb)
            delete[] vb;
        if (ib)
            delete[] ib;
    }

    ~SM_TexCoordTrans() { Clear(); }

    VertexT* vb;
    WORD*    ib;
    int      xNum;
    int      yNum;
};
}  // namespace

//
// function : build static terrain shadow map
// author   : zxy
//

void RtScene::BuildShadowMap() {
    for (int y = 0; y < m_iBlockYCnt; ++y)
        for (int x = 0; x < m_iBlockXCnt; ++x)
            BuildShadowMap(Int2(x, y));
}

void RtScene::BuildShadowMap_ForCurrBlock() {
    int _blockx = 0;
    int _blocky = 0;

    GetBlockMapbyPos(RtgVertex2(m_pDevice->m_pCamera->m_vEyePt.x, m_pDevice->m_pCamera->m_vEyePt.y),
                     _blockx, _blocky);
    BuildShadowMap(Int2(_blockx, _blocky));
}

void RtScene::BuildShadowMap(const Int2& _blockIndex) {
    RtsSceneBlockMap* _blockMap = GetBlockMapbyIndex(_blockIndex.x, _blockIndex.y);

    if (!_blockMap || !(*_blockMap->m_szFileName))
        return;

    RtGetRender()->GetTextMgr()->EnableMutiThreadLoad(false);
    RtGetRender()->IncreaseRenderFrame();
    RequestBlockReadyForSM(Int2(_blockIndex.x, _blockIndex.y));

    RtSceneBlock* pBlock = (RtSceneBlock*)_blockMap->m_pBlock;
    m_pDevice->SetLight(m_iCustomLightCount, &m_lightDirect);

    RtgCamera* camMain = m_pDevice->m_pCamera;
    RtgCamera  camSM;
    RtgShader  shaSM;
    const int  sizeSMMap = 512;
    const int  sizeSM    = 4096;

    BOOL fogEnable = m_pDevice->GetFogEnable();
    m_pDevice->SetFogEnable(false);

    const RtgVertex3& vsun = m_lightDirect.vDirection;

    RtgTextItem* texColor = RtGetRender()->GetTextMgr()->CreateTexture(
        sizeSM, sizeSM, 1, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, D3DUSAGE_RENDERTARGET);
    RtgTextItem* texDepth = RtGetRender()->GetTextMgr()->CreateTexture(
        sizeSM, sizeSM, 1, D3DFMT_D16, D3DPOOL_DEFAULT, D3DUSAGE_DEPTHSTENCIL);
    RtgTextItem* texTemp = RtGetRender()->GetTextMgr()->CreateTexture(
        sizeSMMap, sizeSMMap, 1, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, D3DUSAGE_RENDERTARGET);

    if (!texColor || !texDepth || !texTemp) {
        RtGetRender()->GetTextMgr()->ReleaseTexture(texColor);
        RtGetRender()->GetTextMgr()->ReleaseTexture(texDepth);
        RtGetRender()->GetTextMgr()->ReleaseTexture(texTemp);
        RtCoreLog().Error("build sm fail, can not create texture\n");
        return;
    }

    SM_TexCoordTrans tct;
    tct.Init(g_iSceneTileCntX, g_iSceneTileCntY, sizeSMMap, sizeSMMap);

    RtgVertex3 vc = (pBlock->m_vMax + pBlock->m_vMin) * 0.5f;
    vc.z          = pBlock->m_vMin.z;

    if (!pBlock->m_texItemSM)
        if (!(pBlock->m_texItemSM =
                  RtGetRender()->GetTextMgr()->CreateTexture(sizeSMMap, sizeSMMap, 1, D3DFMT_L8)))
            return;

    float rg =
        sqrtf((pBlock->m_vMax.x - pBlock->m_vMin.x) * (pBlock->m_vMax.x - pBlock->m_vMin.x) +
              (pBlock->m_vMax.y - pBlock->m_vMin.y) * (pBlock->m_vMax.y - pBlock->m_vMin.y)) *
        0.8f;

    camSM.SetOrthoParams(-rg, -rg, rg, rg, 0.f, 100000.f);
    camSM.SetViewParams(vc - vsun * 10000.f, vc, RtgVertex3(0.0f, 0.0f, 1.0f));
    camSM.UpdateMatrix();

    RtgMatrix16 uvMat = camSM.GetViewMatrix() * camSM.GetProjMatrix();
    m_pDevice->SetCamera(&camSM);
    m_pDevice->SetMatrixFromCamera();

    RtGetRender()->IncreaseRenderFrame();
    PreRender(camSM, pBlock);
    m_pDevice->SetRenderTarget(texColor, texDepth);
    m_pDevice->ClearRenderTarget(false, true, true, 0x00000000);
    m_pDevice->BeginScene();
    RenderSceneObject(camSM, RTGRM_NOBLENDOBJ_ONLY);
    m_pDevice->EndScene();
    m_pDevice->RestoreRenderTarget();

    RtgVertex4 vsun4 = RtgVertex4(vsun.x, vsun.y, vsun.z, 1.f) * uvMat;
    float      scal  = 1.f / sizeSM;
    RtgVertex2 uvOft = RtgVertex2((vsun4.x / vsun4.w * 0.5f + 0.5f) * scal,
                                  (1.f - (vsun4.y / vsun4.w * 0.5f + 0.5f)) * scal);

    for (int i = 0; i < g_iSceneTileCntX * g_iSceneTileCntY * 4; ++i) {
        RtsSVertex& v  = pBlock->m_pVertices[i];
        RtgVertex4  v4 = RtgVertex4(v.pPos->v.x, v.pPos->v.y, v.pPos->v.z, 1.f) * uvMat;
        tct.SetUV(i, v4.x / v4.w * 0.5f + 0.5f + uvOft.x,
                  1.f - (v4.y / v4.w * 0.5f + 0.5f) + uvOft.y);
        v.uvSM.Set(v4.x / v4.w * 0.5f + 0.5f + uvOft.x,
                   1.f - (v4.y / v4.w * 0.5f + 0.5f) + uvOft.y);
    }

    m_pDevice->SetRenderTarget(texTemp, texDepth);
    m_pDevice->ClearRenderTarget(false, true, true, 0x00000000);
    m_pDevice->BeginScene();
    tct.Render(m_pDevice, texColor);
    m_pDevice->EndScene();
    m_pDevice->RestoreRenderTarget();
    ProcessSM(texTemp, sizeSMMap, pBlock->m_texItemSM, sizeSMMap);

    pBlock->SaveSM2File();

    m_pDevice->SetCamera(camMain);
    m_pDevice->SetMatrixFromCamera();
    m_pDevice->SetFogEnable(fogEnable);

    RtGetRender()->GetTextMgr()->ReleaseTexture(texColor);
    RtGetRender()->GetTextMgr()->ReleaseTexture(texDepth);
    RtGetRender()->GetTextMgr()->ReleaseTexture(texTemp);

    RtGetRender()->IncreaseRenderFrame();
    RtGetRender()->GetTextMgr()->EnableMutiThreadLoad(true);
}

void RtScene::ViewCenter() {
    RtSceneBlock* pBlock = GetBlockByPos(m_pDevice->m_pCamera->m_vEyePt);

    if (!pBlock)
        return;

    RtgVertex3 vc = (pBlock->m_vMax + pBlock->m_vMin) * 0.5f;
    vc.z          = pBlock->m_vMin.z;
    vc.y -= 2900;
    vc.z += 2000.f;
    m_pDevice->m_pCamera->SetPosition(vc);
    m_pDevice->m_pCamera->SetPitch(0.f);
    m_pDevice->m_pCamera->SetYaw(0.f);
    m_pDevice->m_pCamera->SetRoll(0.8f);
}

/* 
  modify by zxy 
  */
bool RtScene::Render(RtgCamera& inCamera, RTGRenderMask eMask) {
    m_pDevice->SetLight(m_iCustomLightCount, &m_lightDirect);

    if (eMask & RTGRM_NOBLENDOBJ_ONLY) {
        PreRenderDynamicLight();

        if (m_bHaveReflect) {
            RenderSceneTile(inCamera);
            RenderSceneMirrorObject(inCamera);
            RenderSceneObject(inCamera, RTGRM_NOBLENDOBJ_ONLY);
        } else {
            RenderSceneObject(inCamera, RTGRM_NOBLENDOBJ_ONLY);
            RenderSceneTile(inCamera);
        }

        m_GrassRender.Render(&inCamera);
    }

    if (eMask & RTGRM_BLENDOBJ_ONLY)
        RenderSceneObjectShadow(inCamera);

    if (eMask & RTGRM_BLENDOBJ_ONLY) {
        m_FootPrint.Render(inCamera, RTGRM_BLENDOBJ_ONLY);
        RenderSceneWater(inCamera);
        RenderSceneObject(inCamera, RTGRM_BLENDOBJ_ONLY);
        WashOutRenderIndex();
    }

    if (RtgGetRenderOption()->bTerrainNormal)
        m_SceneRender.RenderNormal(this);

    m_bSceneModify = false;

    if ((eMask & RTGRM_BLENDOBJ_ONLY) && m_bShowBlockFlag && (!m_bRenderMiniMapMode)) {
        if (m_fShowBlockFlagTime > 0.f) {
            if ((s_fBlockFlagShowTime - m_fShowBlockFlagTime) < s_fBlockFlagShowTimeIn)
                m_shadowBlockFlag.dwTextureFactor =
                    (((int)((s_fBlockFlagShowTime - m_fShowBlockFlagTime) / s_fBlockFlagShowTimeIn *
                            255.f))
                     << 24) |
                    0x00FFFFFF;
            else if (m_fShowBlockFlagTime < s_fBlockFlagShowTimeOut)
                m_shadowBlockFlag.dwTextureFactor =
                    (((int)(m_fShowBlockFlagTime / s_fBlockFlagShowTimeOut * 255.f)) << 24) |
                    0x00FFFFFF;
            else
                m_shadowBlockFlag.dwTextureFactor = 0xffffffff;

            RECT          rt;
            RtgNVertexVCT pNV[4];
            DWORD         dwColor = 0xffffffff;
            RtgTextItem*  texItem = m_shadowBlockFlag.Shaders[0].GetTexture();

            rt.left   = m_pDevice->m_iWndWidth / 2 - texItem->width / 2;
            rt.top    = m_pDevice->m_iWndHeight / 3 - texItem->height / 2;
            rt.right  = rt.left + texItem->width;
            rt.bottom = rt.top + texItem->height;

            pNV[0] = RtgNVertexVCT(rt.left, rt.top, dwColor, 0.f, 0.f);
            pNV[1] = RtgNVertexVCT(rt.left, rt.bottom, dwColor, 0.f, 1.f);
            pNV[2] = RtgNVertexVCT(rt.right, rt.bottom, dwColor, 1.f, 1.f);
            pNV[3] = RtgNVertexVCT(rt.right, rt.top, dwColor, 1.f, 0.f);

            m_pDevice->SetDefaultMaterial();
            BOOL bBakFog = m_pDevice->GetFogEnable();
            m_pDevice->SetFogEnable(FALSE);
            m_pDevice->SetRenderState(RTGRS_LIGHTING, FALSE);
            m_pDevice->SetVertexFormat(RTG_TRANSFORMED_VCT);
            m_pDevice->SetShader(&m_shadowBlockFlag);
            m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, &pNV, sizeof(RtgNVertexVCT), 2);
            m_pDevice->SetFogEnable(bBakFog);
            m_pDevice->RestoreShader(&m_shadowBlockFlag);
        }
    }

    return true;
}

/* 
  add by zxy
 */

void RtScene::RecreateGrass(RtSceneBlock* block) {
    if (!block)
        return;

    int grid_num =
        (g_iSceneTileCntX / RTS_BLOCK_GRID_SIZE) * (g_iSceneTileCntY / RTS_BLOCK_GRID_SIZE);

    for (int i = 0; i < grid_num; ++i) {
        RtsSGrid* pGrid = &block->m_pGrid[i];

        for (int y = 0; y < RTS_BLOCK_GRID_SIZE; ++y) {
            for (int x = 0; x < RTS_BLOCK_GRID_SIZE; ++x) {
                int          tileId  = ((y + pGrid->y) * g_iSceneTileCntX + (x + pGrid->x));
                RtsSTileMap* tileMap = pGrid->pBlock->m_pTileMapIdx + tileId;

                if (tileMap->grassDensity > 0) {
                    ClearGrass(tileMap);
                    CreateGrass(tileMap, pGrid->pBlock, pGrid->pBlock->m_pVertices + tileId * 4,
                                tileMap->grassId, tileMap->grassDensity, tileMap->minH,
                                tileMap->maxH, tileMap->aspectW);
                }
            }
        }
    }
}

void RtScene::CreateGrass(RtsSTileMap* tile, RtSceneBlock* block, RtsSVertex* v, WORD tid,
                          WORD density, float minH, float maxH, float aspect) {
    RTASSERT(block);
    RTASSERT(tile);
    RTASSERT(v);
    RTASSERT(!tile->grassVertex);
    RTASSERT(density > 0);

    //tile->grassVertex = (RtgVertexGrass*)RT_NEW char[density * sizeof(RtgVertexGrass) * 8];
    tile->grassVertex  = RT_NEW RtgVertexGrass[density * 8];
    tile->grassId      = tid;
    tile->minH         = minH;
    tile->maxH         = maxH;
    tile->aspectW      = aspect;
    tile->grassDensity = density;

    RtsSceneBlockMap* blockMap = GetBlockMap(block->m_iBlockPosX, block->m_iBlockPosY);

    if (!blockMap)
        return;

    RtgVertex3 vn0 = v[0].pPos->n;
    RtgVertex3 vn1 = v[1].pPos->n;
    RtgVertex3 vn2 = v[2].pPos->n;
    RtgVertex3 vn3 = v[3].pPos->n;

    if (vn0.IfZero())
        vn0 = RtgVertex3(0.f, 0.f, 1.f);

    if (vn1.IfZero())
        vn1 = RtgVertex3(0.f, 0.f, 1.f);

    if (vn2.IfZero())
        vn2 = RtgVertex3(0.f, 0.f, 1.f);

    if (vn3.IfZero())
        vn3 = RtgVertex3(0.f, 0.f, 1.f);

    RtgVertex3      n0 = (vn0 + vn1 + vn2) / 3.f;
    RtgVertex3      n1 = (vn0 + vn2 + vn3) / 3.f;
    RtgVertex4      uv;
    RtgVertexGrass* gv = tile->grassVertex;

    m_GrassRender.CalcGrassTextCoord(&uv, tile->grassId);

    for (int i = 0; i < density; ++i) {
        float f0 = (float)rand() / (float)RAND_MAX;
        float f1 = (float)rand() / (float)RAND_MAX;
        float fh = (float)rand() / (float)RAND_MAX;

        RtgVertex3 pos;
        RtgVertex3 dir;

        if (rand() > RAND_MAX / 2) {
            pos = v[0].pPos->v +
                  ((v[1].pPos->v + (v[2].pPos->v - v[1].pPos->v) * f0) - v[0].pPos->v) * f1;
            dir = n0;
        } else {
            pos = v[1].pPos->v +
                  ((v[2].pPos->v + (v[3].pPos->v - v[2].pPos->v) * f0) - v[1].pPos->v) * f1;
            dir = n1;
        }

        dir.Normalize();

        float szh = minH + fh * (maxH - minH);
        float szw = szh * aspect;
        float rad = PI * 2.f * (float)rand() / (float)RAND_MAX;

        RtgVertex3 rgt[] = {RtgVertex3(cosf(rad), sinf(rad), 0.f),
                            RtgVertex3(cosf(rad + PI * 0.5f), sinf(rad + PI * 0.5f), 0.f)};

        RtgVertex3 _Color0 =
            dir.Dot(-m_lightDirect.vDirection) * RtgColorToVector(blockMap->m_dwSunLighting) +
            RtgColorToVector(blockMap->m_dwEnvironmentLighting);
        RtgVertex3 _Color1;

        for (int j = 0; j < 2; ++j) {
            gv->v = pos - rgt[j] * szw * 0.5f;
            GetTerrainColor(gv->v.x, gv->v.y, _Color1);
            gv->c = RtgVectorToColor(_Color0 + _Color1, 1.f);
            gv->t.Set(uv.x, uv.w);
            ++gv;
            gv->v = pos - rgt[j] * szw * 0.5f + dir * szh;
            GetTerrainColor(gv->v.x, gv->v.y, _Color1);
            gv->c = RtgVectorToColor(_Color0 + _Color1, 1.f);
            gv->t.Set(uv.x, uv.y);
            ++gv;
            gv->v = pos + rgt[j] * szw * 0.5f + dir * szh;
            GetTerrainColor(gv->v.x, gv->v.y, _Color1);
            gv->c = RtgVectorToColor(_Color0 + _Color1, 1.f);
            gv->t.Set(uv.z, uv.y);
            ++gv;
            gv->v = pos + rgt[j] * szw * 0.5f;
            GetTerrainColor(gv->v.x, gv->v.y, _Color1);
            gv->c = RtgVectorToColor(_Color0 + _Color1, 1.f);
            gv->t.Set(uv.z, uv.w);
            ++gv;
        }
    }
}

void RtScene::ClearGrass(RtsSTileMap* tile) {
    DEL_ARRAY(tile->grassVertex);
}

void RtScene::UpdateRenderGridList(RtgCamera& _cam) {
    m_listRenderGrid.clear();

    if (m_iBlockXCnt == 0 || m_iBlockYCnt == 0)
        return;

    RtsSceneBlockMap* blockMap = NULL;
    RtgVertex3        _Pos     = _cam.GetEyePt();
    int               _Indexx  = 0;
    int               _Indexy  = 0;
    Int2              _plIndex[8];
    bool              _plForce[8];
    int               _plIndexNum = 0;

    GetBlockInfo(RtgVertex2(_Pos.x, _Pos.y), _Indexx, _Indexy, _plIndex, _plForce, _plIndexNum);

    if ((blockMap = GetBlockMapbyIndex(_Indexx, _Indexy)) &&
        blockMap->GetState() == Object_State_Ready)
        ((RtSceneBlock*)blockMap->m_pBlock)->GetRenderGrid(&m_listRenderGrid, _cam);

    for (size_t i = 0; i < _plIndexNum; ++i) {
        if ((blockMap = GetBlockMapbyIndex(_plIndex[i].x, _plIndex[i].y)) &&
            blockMap->GetState() == Object_State_Ready &&
            _cam.CheckVisable(((RtSceneBlock*)blockMap->m_pBlock)->m_vMin,
                              ((RtSceneBlock*)blockMap->m_pBlock)->m_vMax)) {
            ((RtSceneBlock*)blockMap->m_pBlock)->GetRenderGrid(&m_listRenderGrid, _cam);
        }
    }
}

bool RtScene::PreRender(RtgCamera& inCamera, RtSceneBlock* pBlock) {
    if (!m_bSceneModify && inCamera.m_matView != m_camMatPrev) {
        m_bSceneModify = true;
        m_camMatPrev   = inCamera.m_matView;
    }

    if (!m_bSceneModify)
        return true;

    RtgVertex3 vSrc = inCamera.m_vEyePt;
    RtgVertex3 vDst = vSrc - inCamera.m_vAxisZ * 10000.f;

    for (size_t i = 0; i < m_vecTransAct.size(); ++i)
        m_vecTransAct[i].act->SetVisible(m_vecTransAct[i].fVisible);
    m_vecTransAct.clear();

    mfBlendDistance         = m_pDevice->GetFogNear();
    mfObjectVisibleDistance = m_pDevice->GetFogFar();

    m_bHaveReflect       = FALSE;
    m_pFirstDynamicLight = NULL;

    m_ObjectRender.Reset();
    m_SceneRender.Reset();
    m_GrassRender.Reset();

    if (pBlock) {
        m_listRenderGrid.clear();
        pBlock->GetRenderGrid(&m_listRenderGrid, inCamera);
    } else {
        UpdateRenderGridList(inCamera);
    }

    if (m_listRenderGrid.empty())
        return true;
    //lyymark 新地图idx偏移
    const int offset = strcmp(this->m_szMapSetName, "scene04") == 0 ? 200 : 0;
    for (gridlist_t::iterator it = m_listRenderGrid.begin(); it != m_listRenderGrid.end(); ++it) {
        RtsSGrid* pGrid = (*it);
        //
        // tile
        //
        if (SupportTerrainRender_PP()) {
            for (int y = 0; y < RTS_BLOCK_GRID_SIZE; ++y) {
                for (int x = 0; x < RTS_BLOCK_GRID_SIZE; ++x) {
                    RtgTextItem* texMap[] = {NULL, NULL, NULL, NULL, NULL};
                    int          i = 0;
                    int          j = 0;

                    int          tileId = ((y + pGrid->y) * g_iSceneTileCntX + (x + pGrid->x));
                    RtsSTileMap* tileMap = pGrid->pBlock->m_pTileMapIdx + tileId;

                    if (pGrid->pBlock->HasSM())
                        texMap[i++] = pGrid->pBlock->m_texItemSM;

                    RTASSERT(tileMap->mapCnt <= 4);
                    while (j < tileMap->mapCnt) {
                        int rmId = ((tileMap->dwSortMapIdx >> (j << 3)) & 0x0ff);
                        rmId += offset;  //add by lyy
                        if (!m_texMaps[rmId].texItem) {
                            m_texMaps[rmId].texItem =
                                RtGetRender()->GetTextMgr()->CreateTextureFromFile(
                                    m_texMaps[rmId].szFile, D3DX_DEFAULT, D3DX_DEFAULT,
                                    D3DX_DEFAULT, D3DFMT_FROM_FILE, D3DPOOL_MANAGED, 0, true);
                            if (!m_texMaps[rmId].texItem) {
                                m_texMaps[rmId].texItem =
                                    RtGetRender()->GetTextMgr()->GetChessboardText();
                                m_texMaps[rmId].texItem->Ref++;
                            }
                        }

                        texMap[i++] = m_texMaps[rmId].texItem;
                        ++j;
                    }

                    m_SceneRender.Insert(pGrid->pBlock, tileId, texMap);

                    // 旧版本的草地处理，如果瓦片有草地且还未生成草地顶点，则创建草地
                    if (tileMap->grassDensity > 0 && !tileMap->grassVertex) {
                        CreateGrass(tileMap, pGrid->pBlock, pGrid->pBlock->m_pVertices + tileId * 4,
                                    tileMap->grassId, tileMap->grassDensity, tileMap->minH,
                                    tileMap->maxH, tileMap->aspectW);
                    }

                    if (tileMap->grassVertex)
                        m_GrassRender.AddGrass(tileMap);
                }
            }
        } else {

            for (int y = 0; y < RTS_BLOCK_GRID_SIZE; ++y) {
                for (int x = 0; x < RTS_BLOCK_GRID_SIZE; ++x) {
                    RtgTextItem* texMap[] = {nullptr};
                    int          i        = 0;
                    int          tileId   = ((y + pGrid->y) * g_iSceneTileCntX + (x + pGrid->x));
                    RtsSTileMap* tileMap  = pGrid->pBlock->m_pTileMapIdx + tileId;

                    while (i < tileMap->mapCnt) {
                        int rmId = ((tileMap->dwSortMapIdx >> (i << 3)) & 0x0ff);

                        if (!m_texMaps[rmId].texItem) {
                            m_texMaps[rmId].texItem =
                                RtGetRender()->GetTextMgr()->CreateTextureFromFile(
                                    m_texMaps[rmId].szFile, D3DX_DEFAULT, D3DX_DEFAULT,
                                    D3DX_DEFAULT, D3DFMT_FROM_FILE, D3DPOOL_MANAGED, 0, true);
                        }

                        texMap[i] = m_texMaps[rmId].texItem;
                        ++i;
                    }

                    if (pGrid->pBlock->HasSM())
                        texMap[i] = pGrid->pBlock->m_texItemSM;

                    m_SceneRender.Insert(pGrid->pBlock, tileId, texMap);
                }
            }
        }

        //
        // object
        //

        if (m_bRenderMiniMapMode) {
            for (int i = 0; i < pGrid->aryObject.size(); ++i) {
                RtsObject* obj = RtObjectManager()->GetObject(pGrid->aryObject[i]);

                if (!obj)
                    continue;

                if (obj->GetLastRenderFrame() >= RtGetRender()->GetRenderFrame())
                    continue;
                obj->SetLastRenderFrame(RtGetRender()->GetRenderFrame());

                if (obj->IsKindOf(RT_RUNTIME_CLASS(RtsActor))) {
                    if (inCamera.CheckVisable(obj->Get_World_AABBox())) {
                        ((RtsActor*)obj)->m_pLights[0].pLight = NULL;
                        if (obj->GetState() == Object_State_New)
                            if (obj->IsKindOf(RT_RUNTIME_CLASS(RtsModel))) {
                                ((RtsModel*)obj)->RecreateResource(pGrid);
                            }
                        m_ObjectRender.AddObject(obj);
                    }
                } else if (obj->IsKindOf(RT_RUNTIME_CLASS(RtsInnervation))) {
                    if (obj->GetState() == Object_State_New)
                        ((RtsInnervation*)obj)->RecreateResource(pGrid);
                    m_ObjectRender.AddObject(obj);
                }
            }
        } else {
            for (int i = 0; i < pGrid->aryObject.size(); ++i) {
                RtsObject* obj = RtObjectManager()->GetObject(pGrid->aryObject[i]);

                if (!obj)
                    continue;

                if (obj->GetLastRenderFrame() >= RtGetRender()->GetRenderFrame())
                    continue;
                obj->SetLastRenderFrame(RtGetRender()->GetRenderFrame());

                if (obj->IsKindOf(RT_RUNTIME_CLASS(RtsActor))) {
                    RtsActor* act = (RtsActor*)obj;
                    if (!act->GetActor())
                        continue;

                    if (!inCamera.CheckVisable(obj->Get_World_AABBox()))
                        continue;

                    act->m_pLights[0].pLight = NULL;

                    BOOL bReflect = FALSE;

                    if (m_bReflectSceneObject || m_bReflectOtherActor) {
                        if (obj->IsKindOf(RT_RUNTIME_CLASS(RtsModel))) {
                            if (m_bReflectSceneObject)
                                bReflect = TRUE;
                        } else {
                            if (m_bReflectOtherActor)
                                bReflect = TRUE;
                        }
                    }

                    if (bReflect &&
                        (GetTerrainMapIdx(obj->GetMatrix()._30, obj->GetMatrix()._31) > 200.f)) {
                        obj->m_pObjectLink = (RtsObject*)1;
                        m_bHaveReflect     = TRUE;
                    } else {
                        obj->m_pObjectLink = NULL;
                    }

                    RtgVertex3* pLookAt = inCamera.GetLookAtPlayerPos();

                    if (pLookAt) {
                        RtgMatrix16 mat2obj = obj->GetMatrix().Inverse();
                        RtgVertex3  s       = vSrc * mat2obj;
                        RtgVertex3  d       = ((*pLookAt) + RtgVertex3(0.f, 0.f, 30.f)) * mat2obj;
                        RtgVertex3  vMin;
                        RtgVertex3  vMax;

                        obj->Get_AABBox()->GetMin(&vMin);
                        obj->Get_AABBox()->GetMax(&vMax);

                        float fDist = 0.f;

                        if (rtgIsLineIntersectAABB(s, d, vMin, vMax, &fDist, NULL)) {
                            if (((RtsActor*)obj)->GetActor()->IsRegIntersect(s, d)) {
                                CRT_ActorInstance* act = ((RtsActor*)obj)->GetActor();
                                m_vecTransAct.push_back(_TransActorHolder(act, act->GetVisible()));
                                act->SetVisible(0.35f);
                            }
                        }
                    }
                }

                m_ObjectRender.AddObject(obj);
            }
        }

        if (pGrid->dLight.size()) {
            for (list<RtsDynamicLight*>::iterator it = pGrid->dLight.begin();
                 it != pGrid->dLight.end(); ++it) {
                (*it)->pNext         = m_pFirstDynamicLight;
                m_pFirstDynamicLight = *it;
            }
        }
    }

    OnAfterPreRender();
    m_ObjectRender.PrepareRender(&inCamera);

    return true;
}

bool RtScene::PreRenderDynamicLight() {
    // 处理动态灯光：先取出灯光列表，然后比较所有的渲染物件，并且把灯光赋值给所有的渲染物件
    /*
    float fD2;
    int i, j, iLight;
    RtsDynamicLight* pLight;
    RtsObject* pObject = m_pRenderObjHead;
    while (pObject)
    {
        if (pObject->IsKindOf(RT_RUNTIME_CLASS(RtsActor)))
        {
            iLight = 0;
            for (pLight=m_pFirstDynamicLight; pLight && iLight<RtsActor::EADL_MAX_DYNAMIC_LIGHT; pLight=(RtsDynamicLight*)pLight->pNext)
            {
                fD2 = pLight->vPosition.Distance2(*((RtgVertex3*)(pObject->GetMatrix().Position())));
                if (fD2<pLight->fRange*pLight->fRange)
                {
                    for (i=0; i<iLight; i++)
                    {
                        if (((RtsActor*)pObject)->m_pLights[i].fDistance>fD2)
                        {
                            j=iLight;
                            if (j>=RtsActor::EADL_MAX_DYNAMIC_LIGHT) j=RtsActor::EADL_MAX_DYNAMIC_LIGHT-1;
                            for (; j>i; j--)
                            {
                                ((RtsActor*)pObject)->m_pLights[j].fDistance = ((RtsActor*)pObject)->m_pLights[j-1].fDistance;
                                ((RtsActor*)pObject)->m_pLights[j].pLight = ((RtsActor*)pObject)->m_pLights[j-1].pLight;
                            }
                            break;
                        }
                    }
                    if (i>=iLight)
                    {
                        ((RtsActor*)pObject)->m_pLights[iLight].fDistance = fD2;
                        ((RtsActor*)pObject)->m_pLights[iLight].pLight = pLight;
                        iLight ++;
                        //LOG1("%d", iLight);
                    }
                }
            }
            if (iLight<RtsActor::EADL_MAX_DYNAMIC_LIGHT)
            {
                ((RtsActor*)pObject)->m_pLights[iLight].pLight = NULL;
            }
        }
        pObject = pObject->m_pPrevRenderObject;
    } */
    return true;
}

bool RtScene::DetectObjectVisiblity(float fDistanceToCamera, RtsModel* pObject) {
    float                   fRealDistance    = fDistanceToCamera - pObject->m_fRadius;
    RtsObject::EObjectSharp eObjSharp        = pObject->m_eObjectSharp;
    float                   fVisibleDistance = 0.0f;
    float                   fBlendDistance   = 0.0f;
    switch (eObjSharp) {
        case RtsObject::EOS_HUGEOBJECT: {
            pObject->m_fBlendFactor = 1.0f;
            return true;
        }
        case RtsObject::EOS_BIGOBJECT: {
            fVisibleDistance = mfObjectVisibleDistance;
            fBlendDistance   = mfBlendDistance;
        } break;
        case RtsObject::EOS_MEDIAOBJECT: {
            float fFactor    = 0.75f;  //0.60f;
            fVisibleDistance = mfObjectVisibleDistance * fFactor;
            fBlendDistance   = mfBlendDistance * fFactor;
        } break;

        case RtsObject::EOS_SMALLOBJECT:
        default: {
            float fFactor    = 0.40f;  //0.25f;
            fVisibleDistance = mfObjectVisibleDistance * fFactor;
            fBlendDistance   = mfBlendDistance * fFactor;
        } break;
    }
    if (fRealDistance >= fVisibleDistance) {
        return false;
    }
    CRT_ActorInstance* pkActorInstance = pObject->GetActor();
    if (fRealDistance < fBlendDistance) {
        pObject->m_fBlendFactor    = 1.0f;
        pkActorInstance->m_visible = pObject->m_fBlendFactor;
    } else {
        float fRealBlendDistance = fVisibleDistance - fBlendDistance;
        if (fRealBlendDistance == 0.0f) {
            fRealBlendDistance = 1.0f;
        }
        pObject->m_fBlendFactor    = (fRealDistance - fBlendDistance) / fRealBlendDistance;
        pObject->m_fBlendFactor    = 1.0f - pObject->m_fBlendFactor;
        pkActorInstance->m_visible = pObject->m_fBlendFactor;
    }
    return true;
}

//bool RtScene::AddRenderObject(const RtgVertex3& vPos, RtsObject* pObject)
//{
//    m_ObjectRender.AddObject(pObject);
//    return true;
//}

void RtScene::RenderSceneWater(RtgCamera& inCamera) {
    RtgDeviceD3D9* dev = RtGetRender();
    RTASSERT(dev);

    RtsSVertex*        pVertex;
    int                iV, iI;
    int                k, iCnt, iiI, sx, sy;
    BYTE*              pV;
    short*             pI;
    int                iCurTile;
    RtsSRenderTile*    pCurTile;
    RtsSRenderMap*     pCurMap;
    WORD               wVertexIndexOffset[6] = {0, 1, 2, 1, 3, 2};
    RtgVertex3         vWaterNormal(0.f, 0.f, 1.f);
    float              fWaterHeight;
    static const int   iWaterUVSpan                                    = 2;
    static const float fWaterUV[iWaterUVSpan + 1][iWaterUVSpan + 1][2] = {
        {{0.0f, 0.0f}, {0.0f, 0.5f}, {0.0f, 1.0f}},
        {{0.5f, 0.0f}, {0.5f, 0.5f}, {0.5f, 1.0f}},
        {{1.0f, 0.0f}, {1.0f, 0.5f}, {1.0f, 1.0f}}};

    // 预处理水面
    PreRenderWater(inCamera);

    if (m_bRenderSceneWater && m_pWaterRenderMapHead) {
        m_bOptionWaterCatoptricSky = FALSE;

        RTGVertexFormat eWaterRenderVertexFormat =
            m_bOptionWaterCatoptricSky ? RTG_VNCT2 : RTG_VNCT;
        DWORD dwWaterRenderVertexStride = sizeof(float) * 8 + sizeof(DWORD);
        if (m_bOptionWaterCatoptricSky)
            dwWaterRenderVertexStride += sizeof(float) * 2;

        iV = m_iWaterRenderTileCur * 4 * dwWaterRenderVertexStride;
        iI = m_iWaterRenderTileCur * 6;

        RtgShareBufferItem vbItem;
        RtgShareBufferItem ibItem;

        pV = (BYTE*)dev->GetBuffMgr()->Lock_Shared_vb(iV, &vbItem);
        pI = (short*)dev->GetBuffMgr()->Lock_Shared_ib(iI * sizeof(short), &ibItem);

        iiI = 0;

        iCnt    = 0;
        pCurMap = m_pWaterRenderMapHead;
        while (pCurMap) {
            // 渲染一个贴图
            pCurMap->iStart = iiI;
            iCurTile        = pCurMap->iTile;
            while (iCurTile >= 0) {
                pCurTile     = m_pRenderTile + iCurTile;
                pVertex      = pCurTile->pBlock->m_pVertices + pCurTile->iIdx * 4;
                sx           = pCurTile->iIdx % g_iSceneTileCntX;
                sy           = pCurTile->iIdx / g_iSceneTileCntX;
                fWaterHeight = pVertex->pPos->fWaterHeight;
                for (k = 0; k < 4; k++) {
                    memcpy(pV, pVertex->pPos, sizeof(RtgVertex3));
                    ((float*)pV)[2] = fWaterHeight;
                    memcpy(pV + sizeof(RtgVertex3), &vWaterNormal, sizeof(RtgVertex3));

                    if (m_bUseDynamicLight) {
                        ((DWORD*)pV)[6] = RtgAddVertexColor2(
                            pVertex->pPos->v, vWaterNormal, pVertex->pPos->c, pCurMap->dwColor,
                            pCurMap->fColorRate, m_pFirstDynamicLight);
                        ((DWORD*)pV)[6] =
                            (((DWORD*)pV)[6] & 0x00FFFFFF) | (pVertex->pPos->cWaterAlphaMap << 24);
                    } else {
                        ((DWORD*)pV)[6] =
                            RtgAddVertexColor2(pVertex->pPos->v, vWaterNormal, pVertex->pPos->c,
                                               pCurMap->dwColor, pCurMap->fColorRate, NULL);
                        ((DWORD*)pV)[6] =
                            (((DWORD*)pV)[6] & 0x00FFFFFF) | (pVertex->pPos->cWaterAlphaMap << 24);
                        //((DWORD*)pV)[6] = (pVertex->pPos->c&0x00FFFFFF) | (pVertex->pPos->cWaterAlphaMap<<24);
                    }

                    memcpy(pV + sizeof(RtgVertexVNC),
                           fWaterUV[(sy) % iWaterUVSpan + k / 2][(sx) % iWaterUVSpan + k % 2],
                           sizeof(float) * 2);
                    if (m_bOptionWaterCatoptricSky) {
                        memcpy(pV + sizeof(RtgVertexVNC) + sizeof(float) * 2,
                               fWaterUV[(sy) % iWaterUVSpan + k / 2][(sx) % iWaterUVSpan + k % 2],
                               sizeof(float) * 2);
                    }
                    pVertex++;
                    pV += dwWaterRenderVertexStride;
                }

                // Index
                for (k = 0; k < 6; k++) {
                    *pI = iCnt * 4 + wVertexIndexOffset[k];
                    iiI++;
                    pI++;
                }

                iCnt++;
                iCurTile = pCurTile->iNext;
            }

            pCurMap->iCount = iiI - pCurMap->iStart;
            pCurMap->iCost  = 0;

            pCurMap = pCurMap->pNext;
        }

        dev->GetBuffMgr()->Unlock(&vbItem);
        dev->GetBuffMgr()->Unlock(&ibItem);
        // dev->SetStreamSource(0, dev->GetBuffMgr()->GetShared_vb(), dwWaterRenderVertexStride);
        // dev->SetIndices(dev->GetBuffMgr()->GetShared_ib());
        dev->SetVertexFormat(eWaterRenderVertexFormat);

        RtgMatrix16 m16;
        m16.Unit();
        dev->SetLight(m_iCustomLightCount + 1, NULL);
        dev->SetMatrix(RTGTS_WORLD, &m16);
        dev->SetDefaultMaterial();
        dev->SetRenderState(RTGRS_LIGHTING, TRUE);
        dev->SetRenderState(RTGRS_EMISSIVEMATERIALSOURCE, RTGMCS_COLOR1);
        //m_pDevice->SetRenderState(RTGRS_AMBIENTMATERIALSOURCE, RTGMCS_MATERIAL);
        dev->SetRenderState(RTGRS_DIFFUSEMATERIALSOURCE, RTGMCS_COLOR1);
        dev->SetRenderState(RTGRS_Z_WRITE, FALSE);
        pCurMap = m_pWaterRenderMapHead;
        while (pCurMap) {
            dev->SetShader(&(pCurMap->Shader));
            dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, &vbItem, dwWaterRenderVertexStride,
                                      &ibItem, pCurMap->iCount / 3, pCurMap->iStart);
            /* dev->DrawIndexedPrimitive(
                D3DPT_TRIANGLELIST,
                0,
                0,
                 m_iWaterRenderTileCur*4, pCurMap->iStart, pCurMap->iCount/3);*/
            dev->RestoreShader(&(pCurMap->Shader));
            pCurMap = pCurMap->pNext;
        }
        m_pDevice->SetRenderState(RTGRS_Z_WRITE, TRUE);
    }
}

void RtScene::RenderSceneObjectShadow(RtgCamera& inCamera) {
    m_ObjectRender.RenderSceneObjectShadow(this, &inCamera);
}

void RtScene::RenderSceneMirrorObject(RtgCamera& inCamera) {
    m_ObjectRender.RenderSceneMirrorObject(this, &inCamera);
}

void RtScene::RenderSceneTile(RtgCamera& inCamera) {
    RtGetPref()->TerrRender = rtMilliseconds();
    if (!m_bRenderScene)
        return;
    m_SceneRender.Render(this);
    RtGetPref()->TerrRender = rtMilliseconds() - RtGetPref()->TerrRender;
}

void RtScene::RenderSceneObject(RtgCamera& inCamera, RTGRenderMask eMask) {
    m_ObjectRender.RenderSceneObjectNormal(this, &inCamera, eMask);
}

void RtScene::RenderSceneObjectHighLight(RtgCamera& inCamera) {
    RTASSERT(FALSE);
    /*
    if (!m_pDevice->IsUsePostProcessObjectHighLight())
    {
        return;
    }

    RtgVertex3 color = RtgColorToVector(m_pDevice->m_dwPostProcessObjectHLColorSpe);
    RtgVertex3 scale(m_pDevice->m_fPostProcessObjectHLScale, m_pDevice->m_fPostProcessObjectHLScale, m_pDevice->m_fPostProcessObjectHLScale);

    // set default render state
    m_pDevice->SetRenderState(RTGRS_LIGHTING, TRUE);
    m_pDevice->SetRenderState(RTGRS_EMISSIVEMATERIALSOURCE, RTGMCS_MATERIAL);
    //m_pDevice->SetRenderState(RTGRS_AMBIENTMATERIALSOURCE, RTGMCS_MATERIAL);
    m_pDevice->SetRenderState(RTGRS_DIFFUSEMATERIALSOURCE, RTGMCS_MATERIAL);

    int iLight;
    RtsObject* pObject;
    rtClock(m_sTimerRenderObject);
    if (m_pRenderObjHead==NULL) return;

    // 从后面渲染到前面
    pObject = m_pRenderObjHead;
    while (pObject)
    {
        if (m_bUseDynamicLight)
        {
            if (pObject->IsKindOf(RT_RUNTIME_CLASS(RtsActor)))
            {
                for (iLight=0; iLight<RtsActor::EADL_MAX_DYNAMIC_LIGHT && ((RtsActor*)pObject)->m_pLights[iLight].pLight; iLight++)
                {
                    m_pDevice->SetLight(iLight+m_iCustomLightCount+1, ((RtsActor*)pObject)->m_pLights[iLight].pLight);
                }
                m_pDevice->SetLight(iLight+m_iCustomLightCount+1, NULL);
            }else
            {
                m_pDevice->SetLight(m_iCustomLightCount+1, NULL);
            }
        }
        pObject->RenderHighLight(inCamera, *m_pDevice, color, scale);
        pObject = pObject->m_pPrevRenderObject;
    }
    rtUnclock(m_sTimerRenderObject);
    */
}

bool RtScene::PreRenderWater(RtgCamera& inCamera) {
    RtsSRenderMap* pRenderMap;
    RtsSTileMap*   pTileMap;
    int            x, y, iIdx;

    m_pWaterRenderMapHead = NULL;
    m_iWaterRenderTileCur = 0;

    for (x = 0; x < MAX_WATER_COUNT; x++) {
        m_WaterRenderMap[x].bUse   = false;
        m_WaterRenderMap[x].iStart = 0;
        m_WaterRenderMap[x].iCount = 0;
        m_WaterRenderMap[x].pNext  = NULL;
        m_WaterRenderMap[x].iTile  = -1;
    }

    for (gridlist_t::iterator it = m_listRenderGrid.begin(); it != m_listRenderGrid.end(); ++it) {
        RtsSGrid* pGrid = (*it);

        // 地表的处理
        for (y = 0; y < RTS_BLOCK_GRID_SIZE; y++) {
            for (x = 0; x < RTS_BLOCK_GRID_SIZE; x++) {
                iIdx     = ((y + pGrid->y) * g_iSceneTileCntX + (x + pGrid->x));
                pTileMap = pGrid->pBlock->m_pTileMapIdx + iIdx;
                if (pTileMap->cWaterType == 0)
                    continue;
                pRenderMap = m_WaterRenderMap + (pTileMap->cWaterType - 1);

                if (!pRenderMap->bUse) {
                    pRenderMap->pNext     = m_pWaterRenderMapHead;
                    pRenderMap->iTile     = -1;
                    pRenderMap->iCost     = 1;
                    pRenderMap->bUse      = true;
                    m_pWaterRenderMapHead = pRenderMap;
                }

                m_pRenderTile[m_iWaterRenderTileCur].pBlock = pGrid->pBlock;
                m_pRenderTile[m_iWaterRenderTileCur].iIdx   = iIdx;
                m_pRenderTile[m_iWaterRenderTileCur].iUV    = 0;
                m_pRenderTile[m_iWaterRenderTileCur].iNext  = pRenderMap->iTile;
                pRenderMap->iTile                           = m_iWaterRenderTileCur;
                m_iWaterRenderTileCur++;
                if (m_iWaterRenderTileCur >= m_iRenderTileCount) {
                    m_iRenderTileCount += 500;
                    RtsSRenderTile* pTempRenderTile = RT_NEW RtsSRenderTile[m_iRenderTileCount];
                    memcpy(pTempRenderTile, m_pRenderTile,
                           sizeof(RtsSRenderTile) * m_iWaterRenderTileCur);
                    DEL_ARRAY(m_pRenderTile);
                    m_pRenderTile = pTempRenderTile;
                }
                if (pRenderMap->bLoaded == false) {
                    pRenderMap->iStart = 0;
                    pRenderMap->iCount = 0;
                    pRenderMap->iCost  = 1;
                    SetupWaterShader(pGrid->pBlock->m_pTileMapIdx[iIdx].cWaterType);
                }
            }
        }
    }
    return true;
}

void RtScene::WashOutRenderIndex() {
    int i;
    /* for (i=0; i<MAX_MAP_COUNT; i++)
    {
        if (m_RenderMap[i].bLoaded)
        {
            m_RenderMap[i].iCost --;
            if (m_RenderMap[i].iCost < MIN_INDEX_COST)
            {
                m_pDevice->DeleteShader(&(m_RenderMap[i].Shader));
                m_RenderMap[i].bLoaded = false;
                m_sCountUseTexture --;
            }
        }
    }*/
    for (i = 0; i < MAX_WATER_COUNT; i++) {
        if (m_WaterRenderMap[i].bLoaded) {
            m_WaterRenderMap[i].iCost--;
            if (m_WaterRenderMap[i].iCost < MIN_INDEX_COST) {
                m_pDevice->DeleteShader(&(m_WaterRenderMap[i].Shader));
                m_WaterRenderMap[i].bLoaded = false;
            }
        }
    }
}

void RtScene::PreRenderIndex() {
    /* for (int i=0; i<MAX_MAP_COUNT; i++)
    {
        if (m_RenderMap[i].bLoaded)
        {
            m_RenderMap[i].bUse = false;
            m_RenderMap[i].iStart = 0;
            m_RenderMap[i].iCount = 0;
            m_RenderMap[i].pNext = NULL;
            m_RenderMap[i].iTile = -1;
        }
    }*/
}

int RtScene::GetTerrainMapIdx(float fX, float fY) {
    RtSceneBlock* pBlock = GetBlockByPos(fX, fY);
    if (pBlock) {
        return pBlock->GetTerrainMapIdx(fX, fY);
    }
    return 0;
}

int RtScene::GetTerrainMapType(float fX, float fY) {
    return (m_texMaps[GetTerrainMapIdx(fX, fY)].iType);
}

const char* RtScene::GetTerrainMapFile(float fX, float fY) {
    return (m_texMaps[GetTerrainMapIdx(fX, fY)].szFile);
}

const char* RtScene::GetTerrainMapName(float fX, float fY) {
    return (m_texMaps[GetTerrainMapIdx(fX, fY)].szName);
}

DWORD RtScene::GetTerrainColor(float fX, float fY) {
    RtSceneBlock* pBlock = GetBlockByPos(fX, fY);
    if (pBlock) {
        return pBlock->GetTerrainColor(fX, fY);
    }
    return 0x0000;
}

void RtScene::GetTerrainColor(float fX, float fY, RtgVertex3& vColor) {
    DWORD dwColor = GetTerrainColor(fX, fY);
    vColor.x      = ((dwColor & 0x00FF0000) >> 16) / 255.f;
    vColor.y      = ((dwColor & 0x0000FF00) >> 8) / 255.f;
    vColor.z      = (dwColor & 0x000000FF) / 255.f;
}

// 如果在场景中有渲染物体pRenderObject，就需要从渲染列表中移出
void RtScene::RemoveObjectByLastRenderList(RtsObject* pRenderObject) {
    m_ObjectRender.DelObject(pRenderObject);
}

bool RtScene::CheckObjectIsLastRenderList(RtsObject* pFindObject) {
    /* CHECK(pFindObject!=NULL);
    RtsObject* pObject = this->m_pRenderObjHead;
    while (pObject)
    {
        if (pObject==pFindObject)
        {
            return true;
        }
        pObject = pObject->m_pPrevRenderObject;
    }
    return false;*/

    return false;
}

bool RtScene::SaveEnvironmentInfo(const char* szFilename) {
    RtArchive* pArc = RtCoreFile().CreateFileWriter(NULL, szFilename, 0);
    if (pArc == NULL)
        return false;

    // Camera info
    *pArc << m_pDevice->m_pCamera->m_matView;
    *pArc << m_pDevice->m_pCamera->m_fFOV;
    *pArc << m_pDevice->m_pCamera->m_fAspect;
    *pArc << m_pDevice->m_pCamera->m_fNearPlane;
    *pArc << m_pDevice->m_pCamera->m_fFarPlane;

    // Sky light
    *pArc << m_lightSkyColor;

    // Sun light
    *pArc << m_lightDirect;
    *pArc << m_lightTerrainAmbient;

    // Fog
    BOOL  bFogEnable = m_pDevice->GetFogEnable();
    DWORD dwFogColor = m_pDevice->GetFogColor();
    float fFogNear   = m_pDevice->GetFogNear();
    float fForFar    = m_pDevice->GetFogFar();
    *pArc << bFogEnable;
    *pArc << dwFogColor;
    *pArc << fFogNear;
    *pArc << fForFar;

    RtCoreFile().CloseFile(pArc);
    return true;
}

bool RtScene::LoadEnvironmentInfo(const char* szFilename) {
    RtArchive* pArc = RtCoreFile().CreateFileReader(szFilename, 0);
    if (pArc == NULL)
        return false;
    RtgMatrix16 m16;

    // Camera info
    *pArc << m16;
    *pArc << m_pDevice->m_pCamera->m_fFOV;
    *pArc << m_pDevice->m_pCamera->m_fAspect;
    *pArc << m_pDevice->m_pCamera->m_fNearPlane;
    *pArc << m_pDevice->m_pCamera->m_fFarPlane;
    m_pDevice->m_pCamera->SetProjParams(
        m_pDevice->m_pCamera->m_fFOV, m_pDevice->m_pCamera->m_fAspect,
        m_pDevice->m_pCamera->m_fNearPlane, m_pDevice->m_pCamera->m_fFarPlane);
    m_pDevice->m_pCamera->SetViewMatrix(m16);

    // Sky light
    DWORD dwColor;
    *pArc << dwColor;
    this->SetSkyLighting(dwColor);

    // Sun light
    *pArc << m_lightDirect;
    *pArc << m_lightTerrainAmbient;

    // Fog
    BOOL  bFogEnable;
    DWORD dwFogColor;
    float fFogNear;
    float fForFar;
    *pArc << bFogEnable;
    *pArc << dwFogColor;
    *pArc << fFogNear;
    *pArc << fForFar;
    m_pDevice->SetFogEnable(bFogEnable);
    m_pDevice->SetFogColor(dwFogColor);
    m_pDevice->SetFogNear(fFogNear);
    m_pDevice->SetFogFar(fForFar);

    RtCoreFile().CloseFile(pArc);
    return true;
}

void RtScene::SetSkyLighting(DWORD dwLightColor) {
    if (m_lightSkyColor == dwLightColor)
        return;
    m_lightSkyColor = dwLightColor;
    m_pDevice->SetRenderState(RTGRS_ENVIRONMENT_LIGHTING, m_lightSkyColor);
}

void RtScene::UpdateSkyLight(int iX, int iY, int iMinute) {
    return;
    int               i;
    RtsSLightSetEnum *pEnum = NULL, *pNext;
    iMinute                 = iMinute % 1440;
    int iHour               = iMinute / 60;
    for (i = 0; i < m_lightSet.iEnumCnt - 1; i++) {
        pEnum = m_lightSet.pEnums + i;
        pNext = m_lightSet.pEnums + i + 1;
        if (iHour >= pEnum->iHour && iHour < pNext->iHour) {
            break;
        }
    }
    if (pEnum == NULL)
        return;

    RtgVertex3 vAmbientColor;
    float      fAlpha = (iMinute - pEnum->iHour * 60) * 1.f / ((pNext->iHour - pEnum->iHour) * 60);

    // 环境光
    vAmbientColor = Lerp(pEnum->vAmbientColor, pNext->vAmbientColor, fAlpha);
    SetSkyLighting(RtgVectorToColor(vAmbientColor, 1.f));

    // 平行光
    m_lightDirect.vDiffuse = Lerp(pEnum->vSunColor, pNext->vSunColor, fAlpha);
    ;
    // m_lightDirect.vSpecular = Lerp(pEnum->vSunSpecular, pNext->vSunSpecular, fAlpha);
    m_lightDirect.vSpecular = RtgVertex3(1.0, 1.0, 1.0);

    // 平行光的旋转（太阳的位置）
    RtgMatrix16 m16;
    m16.Unit();
    m16.RotateLZ(DegreeToRadian(0.f));
    m16.RotateLX(DegreeToRadian(60.f + 180.f));
    /*
    float fInclination, fMinInclination = 110.f, fMaxInclination = 250.f;
    m16.RotateLZ(DegreeToRadian(30.f));
    if (iMinute>=720)
    {
    fInclination = ((1440-iMinute)/720.f)*(fMaxInclination-fMinInclination)+fMinInclination;
    }else
    {
    fInclination = (iMinute/720.f)*(fMaxInclination-fMinInclination)+fMinInclination;
    }
    m16.RotateLX(DegreeToRadian(fInclination));
    */

    //LOG3("时间: %2d:%02d, 角度. %3.3f\n", iHour, iMinute%60, fInclination);
    m16.GetRow(2, m_lightDirect.vDirection);
    m_lightDirect.vDirection.x = 1.0;
    m_lightDirect.vDirection.y = 1.0;
    m_lightDirect.vDirection.z = 1.0;

    // 地表的颜色偏移
    m_lightTerrainAmbient = Lerp(pEnum->vTerrainAmbientColor, pNext->vTerrainAmbientColor, fAlpha);
}

// 更新一定区域内的网格
//    iMinX, iMinY, iMaxX, iMaxY 是显示网格的位置, 包含Min不包含Max
void RtScene::UpdateGrid(int iMinX, int iMinY, int iMaxX, int iMaxY) {
    int x, y, xf, yf, iBMinX, iBMinY, iBMaxX, iBMaxY;
    OffsetToMap(iMinX, iMinY);
    OffsetToMap(iMaxX, iMaxY);
    iBMinX = Clamp((iMinX / g_iSceneTileCntX), 0, m_iBlockXCnt);
    iBMinY = Clamp((iMinY / g_iSceneTileCntX), 0, m_iBlockYCnt);
    iBMaxX = Clamp(((iMaxX - 1) / g_iSceneTileCntX) + 1, 0, m_iBlockXCnt);
    iBMaxY = Clamp(((iMaxY - 1) / g_iSceneTileCntX) + 1, 0, m_iBlockYCnt);
    RtSceneBlock* pBlock;
    for (y = iBMinY; y < iBMaxY; y++) {
        for (x = iBMinX; x < iBMaxX; x++) {
            pBlock = (RtSceneBlock*)m_blockMapArray[y * m_iBlockXCnt + x].m_pBlock;
            if (pBlock) {
                xf = x * g_iSceneTileCntX;
                yf = y * g_iSceneTileCntY;
                pBlock->UpdateGrid(iMinX - xf, iMinY - yf, iMaxX - xf, iMaxY - yf);
            }
        }
    }
}

void RtScene::UpdateVertexPositionByHeightmap(int iMinX, int iMinY, int iMaxX, int iMaxY) {
    int x, y, xf, yf, iBMinX, iBMinY, iBMaxX, iBMaxY;
    OffsetToMap(iMinX, iMinY);
    OffsetToMap(iMaxX, iMaxY);
    iBMinX = Clamp((iMinX / g_iSceneTileCntX), 0, m_iBlockXCnt);
    iBMinY = Clamp((iMinY / g_iSceneTileCntX), 0, m_iBlockYCnt);
    iBMaxX = Clamp(((iMaxX - 1) / g_iSceneTileCntX) + 1, 0, m_iBlockXCnt);
    iBMaxY = Clamp(((iMaxY - 1) / g_iSceneTileCntX) + 1, 0, m_iBlockYCnt);
    RtSceneBlock* pBlock;
    for (y = iBMinY; y < iBMaxY; y++) {
        for (x = iBMinX; x < iBMaxX; x++) {
            pBlock = (RtSceneBlock*)m_blockMapArray[y * m_iBlockXCnt + x].m_pBlock;
            if (pBlock) {
                xf = x * g_iSceneTileCntX;
                yf = y * g_iSceneTileCntY;
                //pBlock->UpdateVertexPositionByHeightmap(iMinX-xf, iMinY-yf, iMaxX-xf, iMaxY-yf);
                pBlock->UpdateVertexPositionByHeightmap();
            }
        }
    }
}

void RtScene::UpdateVertexPositionByTerrainHeightmap(int iMinX, int iMinY, int iMaxX, int iMaxY) {
    int x, y, xf, yf, iBMinX, iBMinY, iBMaxX, iBMaxY;
    OffsetToMap(iMinX, iMinY);
    OffsetToMap(iMaxX, iMaxY);
    iBMinX = Clamp((iMinX / g_iSceneTileCntX), 0, m_iBlockXCnt);
    iBMinY = Clamp((iMinY / g_iSceneTileCntX), 0, m_iBlockYCnt);
    iBMaxX = Clamp(((iMaxX - 1) / g_iSceneTileCntX) + 1, 0, m_iBlockXCnt);
    iBMaxY = Clamp(((iMaxY - 1) / g_iSceneTileCntX) + 1, 0, m_iBlockYCnt);
    RtSceneBlock* pBlock;
    for (y = iBMinY; y < iBMaxY; y++) {
        for (x = iBMinX; x < iBMaxX; x++) {
            pBlock = (RtSceneBlock*)m_blockMapArray[y * m_iBlockXCnt + x].m_pBlock;
            if (pBlock) {
                xf = x * g_iSceneTileCntX;
                yf = y * g_iSceneTileCntY;
                //pBlock->UpdateVertexPositionByTerrainHeightmap(iMinX-xf, iMinY-yf, iMaxX-xf, iMaxY-yf);
                pBlock->UpdateVertexPositionByTerrainHeightmap();
            }
        }
    }
}

RtsObject* RtScene::PickLine(const RtgVertex3& v0, const RtgVertex3& v1, float& fOutDist,
                             int& iOutX, int& iOutY, DWORD dwType) {
    RtgVertex3 dir = v1 - v0;
    float      fLen2;
    RtsObject* pObj;
    fLen2 = dir.Length2();
    dir.Normalize();
    if (pObj = PickRay(v0, dir, fOutDist, iOutX, iOutY, dwType)) {
        if (fOutDist * fOutDist < fLen2)
            return pObj;
    }
    return NULL;
}

RtsObject* RtScene::PickRay(const RtgVertex3& v, const RtgVertex3& dir, float& fOutDist, int& iOutX,
                            int& iOutY, DWORD dwType) {
    if (dwType & (PICK_OBJECT | PICK_HELPER)) {
        RtGetRender()->IncreaseRenderFrame();
    }

    float fDist;
    int   iX, iY;
    fOutDist = 1e10;
    RtsObject *   pObject, *pMinObj = NULL;
    RtSceneBlock* pBlock = GetFirstBlock();
    while (pBlock) {
        pObject = pBlock->PickRay(v, dir, fDist, iX, iY, dwType);
        if (pObject != NULL) {
            if (fDist < fOutDist) {
                fOutDist = fDist;
                pMinObj  = pObject;
                if (dwType & PICK_VTERRAIN) {
                    iOutX = iX + pBlock->m_iBlockPosX * g_iSceneTileCntX;
                    iOutY = iY + pBlock->m_iBlockPosY * g_iSceneTileCntY;
                } else {
                    iOutX = iX + pBlock->m_iBlockPosX * g_iSceneTerrainCntX;
                    iOutY = iY + pBlock->m_iBlockPosY * g_iSceneTerrainCntY;
                }
            }
        }
        pBlock = GetNextBlock(pBlock);
    }
    return pMinObj;
}

bool RtScene::GetHeight(const RtgVertex3& v, float& fOutDist, int& iOutX, int& iOutY,
                        DWORD dwType) {
    RtSceneBlock* pBlock = GetBlockByPos(v);
    if (pBlock) {
        bool bResult = pBlock->GetHeight(v, fOutDist, iOutX, iOutY, dwType);
        iOutX += pBlock->m_iBlockPosX * g_iSceneTileCntX;
        iOutY += pBlock->m_iBlockPosY * g_iSceneTileCntY;
        return bResult;
    }
    return false;
}

bool RtScene::GetTerrainHeight(float fX, float fY, float& outZ) {
    RtgVertex3 v(fX, fY, 0.f);
    int        iOutX, iOutY;
    bool       bResult = GetHeight(v, outZ, iOutX, iOutY, PICK_TERRAIN);
    if (bResult) {
        outZ = 0.f - outZ;
    }
    return bResult;
}

void RtScene::GetPosByTile(int iTileX, int iTileY, RtgVertex3& vPos) {
    GetPosByTileXY(iTileX, iTileY, vPos.x, vPos.y);
    RtgVertex3 v(vPos.x, vPos.y, 0.f);
    int        iOutX, iOutY;
    float      fOutZ;
    bool       bResult = GetHeight(v, fOutZ, iOutX, iOutY, PICK_TERRAIN);
    if (bResult) {
        vPos.z = 0.f - fOutZ;
    }
}

void RtScene::GetPosByTerrain(int iTerrainX, int iTerrainY, RtgVertex3& vPos) {
    GetPosByTerrainXY(iTerrainX, iTerrainY, vPos.x, vPos.y);
    GetTerrainHeight(vPos.x, vPos.y, vPos.z);
}

void RtScene::UpdateAudioListener(const RtgVertex3& vPos) {
    g_pSoundMgr->BeginRender();
    int        x, y, xi, yi, xa, ya, c = 300.f / (RTS_BLOCK_GRID_SIZE * g_fSceneTileWidth) + 1;
    RtsSGrid*  pGrid;
    RtsObject* pObject;
    float      fX, fY;

    fX = vPos.x;
    fY = vPos.y;
    OffsetToMap(fX, fY);
    x  = fX / (RTS_BLOCK_GRID_SIZE * g_fSceneTileWidth);
    y  = fY / (RTS_BLOCK_GRID_SIZE * g_fSceneTileWidth);
    xi = x - c;
    yi = y - c;
    xa = x + c;
    ya = y + c;

    for (y = yi; y <= ya; y++) {
        for (x = xi; x <= xa; x++) {
            pGrid = GetGrid(x, y, false);
            if (pGrid) {
                for (int i = 0; i < pGrid->aryObject.size(); i++) {
                    pObject = RtObjectManager()->GetObject(pGrid->aryObject[i]);
                    if (pObject && pObject->IsKindOf(RT_RUNTIME_CLASS(RtsAudio3D))) {
                        if (((RtsAudio3D*)pObject)->m_pSound != NULL) {
                            g_pSoundMgr->Render(((RtsAudio3D*)pObject)->m_pSound);
                        }
                    }
                }
            }
        }
    }
    g_pSoundMgr->EndRender();
}

void RtScene::GetSortTileMapIdx(RtsSTileMap& tileMapIdx) {
    BYTE bit[4], btmp;
    int  i, j;

    // Sort
    bit[0] = tileMapIdx.mtl[0];
    bit[1] = tileMapIdx.mtl[1];
    bit[2] = tileMapIdx.mtl[2];
    bit[3] = tileMapIdx.mtl[3];

    for (j = 3; j > 0; j--) {
        for (i = 0; i < j; i++) {
            if (bit[i] > bit[i + 1]) {
                btmp       = bit[i];
                bit[i]     = bit[i + 1];
                bit[i + 1] = btmp;
            }
        }
    }

    for (i = 1; i < 4; i++) {
        if (bit[i] == bit[i - 1])
            break;
    }

    if (i != 4) {
        for (j = i + 1; j < 4; j++) {
            if (bit[i] != bit[j]) {
                bit[i] = bit[j];
                i++;
            }
        }
        for (j = i - 1; i < 4; i++) {
            bit[i] = bit[j];
        }
    }

    tileMapIdx.dwSortMapIdx = ((bit[3] & 0x0FF) << 24) | ((bit[2] & 0x0FF) << 16) |
                              ((bit[1] & 0x0FF) << 8) | (bit[0] & 0x0FF);

    tileMapIdx.mapCnt = 1;
    if (bit[0] != bit[1])
        tileMapIdx.mapCnt++;
    if (bit[1] != bit[2])
        tileMapIdx.mapCnt++;
    if (bit[2] != bit[3])
        tileMapIdx.mapCnt++;
    if (tileMapIdx.mapCnt == 3) {
        tileMapIdx.mapCnt = 3;
    }

    BYTE uv[4];
    for (i = 0; i < 4; i++) {
        uv[i] = 0;
        if (tileMapIdx.mtl[0] == bit[i])
            uv[i] |= 0x01;
        if (tileMapIdx.mtl[1] == bit[i])
            uv[i] |= 0x02;
        if (tileMapIdx.mtl[2] == bit[i])
            uv[i] |= 0x04;
        if (tileMapIdx.mtl[3] == bit[i])
            uv[i] |= 0x08;
    }

    tileMapIdx.uv1 = uv[0];
    tileMapIdx.uv2 = uv[1];
    tileMapIdx.uv3 = uv[2];
    tileMapIdx.uv4 = uv[3];

    if (tileMapIdx.mapCnt == 1) {
        if (rand() % 3) {
            tileMapIdx.uv1 = (rand() % 2) ? 15 : 0;
            tileMapIdx.uv2 = 0;
        } else {
            tileMapIdx.uv1 = rand() % 16;
            tileMapIdx.uv2 = 1;
        }
    }

    //tileMapIdx.grassDensity = 0;
}

RtsSGrid* RtScene::GetGridByPos(float fPosX, float fPosY) {
    int iGx, iGy;
    GetGridByPos(fPosX, fPosY, iGx, iGy);
    return GetGrid(iGy, iGy, false);
}

bool RtScene::GetGridByPos(RtgVertex3 vPos, int& iGx, int& iGy) {
    float x, y;
    x = vPos.x;
    y = vPos.y;
    OffsetToMap(x, y);
    iGx = x / (g_fSceneTileWidth * RTS_BLOCK_GRID_SIZE);
    iGy = y / (g_fSceneTileWidth * RTS_BLOCK_GRID_SIZE);
    if (iGx < 0 || iGy < 0)
        return false;
    return true;
}

RtSceneBlock* RtScene::GetBlockByPos(float fPosX, float fPosY) {
    int iBx, iBy;
    OffsetToMap(fPosX, fPosY);
    if (fPosX < 0.f || fPosY < 0.f)
        return NULL;
    iBx = (int)(fPosX / (g_fSceneBlockWidth));
    iBy = (int)(fPosY / (g_fSceneBlockHeight));
    if (iBx >= m_iBlockXCnt || iBy >= m_iBlockYCnt)
        return NULL;
    RtsSceneBlockMap* pBlockMap = &m_blockMapArray[iBy * m_iBlockXCnt + iBx];
    if (pBlockMap->GetState() != Object_State_Ready)
        return NULL;
    return (RtSceneBlock*)pBlockMap->m_pBlock;
}

RtSceneBlock* RtScene::GetBlockByPos(const RtgVertex3& vPos) {
    return GetBlockByPos(vPos.x, vPos.y);
}

RtSceneBlock* RtScene::GetTerrainByPos(float fPosX, float fPosY, int& iTx, int& iTy) {
    int iBx, iBy;
    OffsetToMap(fPosX, fPosY);
    if (fPosX < 0.f || fPosY < 0.f)
        return NULL;
    iBx = (int)(fPosX / (g_fSceneBlockWidth));
    iBy = (int)(fPosY / (g_fSceneBlockHeight));
    if (iBx >= m_iBlockXCnt || iBy >= m_iBlockYCnt)
        return NULL;
    RtsSceneBlockMap* pBlockMap = &m_blockMapArray[iBy * m_iBlockXCnt + iBx];
    if (pBlockMap->GetState() != Object_State_Ready)
        return NULL;
    iTx = (int)(fPosX / g_fSceneTerrainTileWidth);
    iTy = (int)(fPosY / g_fSceneTerrainTileWidth);
    OffsetTerrainToWorld(iTx, iTy);
    return (RtSceneBlock*)pBlockMap->m_pBlock;
}

// 取得网格
RtSceneBlock* RtScene::GetTileByPos(float fPosX, float fPosY, int& iTx, int& iTy) {
    int iBx, iBy;
    OffsetToMap(fPosX, fPosY);
    if (fPosX < 0.f || fPosY < 0.f)
        return NULL;
    iBx = (int)(fPosX / (g_fSceneBlockWidth));
    iBy = (int)(fPosY / (g_fSceneBlockHeight));
    if (iBx >= m_iBlockXCnt || iBy >= m_iBlockYCnt)
        return NULL;
    RtsSceneBlockMap* pBlockMap = &m_blockMapArray[iBy * m_iBlockXCnt + iBx];
    if (pBlockMap->GetState() != Object_State_Ready)
        return NULL;
    iTx = (int)(fPosX / g_fSceneTileWidth);
    iTy = (int)(fPosY / g_fSceneTileWidth);
    OffsetToWorld(iTx, iTy);
    return (RtSceneBlock*)pBlockMap->m_pBlock;
}

RtSceneBlock* RtScene::GetTileByPos(RtgVertex3& vPos, int& iTx, int& iTy) {
    return GetTileByPos(vPos.x, vPos.y, iTx, iTy);
}

RtSceneBlock* RtScene::GetFirstBlock() {
    return (RtSceneBlock*)GetNextBlockTerrain(NULL);
}

RtSceneBlock* RtScene::GetNextBlock(RtSceneBlock* pCur) {
    return (RtSceneBlock*)GetNextBlockTerrain(pCur);
}

// x, y为顶点位置
RtsSVertexValue* RtScene::GetVertexValue(int x, int y, bool bEdit) {
    int iBx, iBy;
    OffsetToMap(x, y);
    if (x < 0 || y < 0)
        return NULL;
    iBx = x / g_iSceneTileCntX;
    iBy = y / g_iSceneTileCntY;
    if (iBx >= m_iBlockXCnt || iBy >= m_iBlockYCnt)
        return NULL;
    RtsSceneBlockMap* pBlockMap = &m_blockMapArray[iBy * m_iBlockXCnt + iBx];
    if (pBlockMap->GetState() != Object_State_Ready)
        return NULL;
    if (bEdit)
        ((RtSceneBlock*)pBlockMap->m_pBlock)->SetModifyFlag(true);
    return ((RtSceneBlock*)pBlockMap->m_pBlock)->m_pPositions +
           ((y - iBy * g_iSceneTileCntY) * (g_iSceneTileCntX + 1) + (x - iBx * g_iSceneTileCntX));
}

BYTE RtScene::GetVertexHeightLevel(int x, int y, bool bTile) {
    RtsSVertexValue* pV = GetVertexValue(x, y, false);
    if (pV) {
        if (bTile)
            return pV->cHeightMap;
        else
            return pV->cTerrainHMap;
    }
    return 0;
}

void RtScene::SetVertexHeightLevel(int x, int y, bool bTerrain, bool bTile, BYTE cHeightLevel) {
    RtsSVertexValue* pV = GetVertexValue(x, y, true);
    if (pV) {
        if (bTerrain)
            pV->cTerrainHMap = cHeightLevel;
        if (bTile)
            pV->cHeightMap = cHeightLevel;
    }
}

RtsSVertex* RtScene::GetBlockVertex(int x, int y) {
    int iBx, iBy;
    OffsetToMap(x, y);
    if (x < 0 || y < 0)
        return NULL;
    iBx = x / g_iSceneTileCntX;
    iBy = y / g_iSceneTileCntY;
    if (iBx >= m_iBlockXCnt || iBy >= m_iBlockYCnt)
        return NULL;
    RtsSceneBlockMap* pBlockMap = &m_blockMapArray[iBy * m_iBlockXCnt + iBx];
    if (pBlockMap->GetState() != Object_State_Ready)
        return NULL;
    return ((RtSceneBlock*)pBlockMap->m_pBlock)->m_pVertices +
           ((y - iBy * g_iSceneTileCntY) * g_iSceneTileCntX + (x - iBx * g_iSceneTileCntX)) * 4;
}

RtSceneBlock* RtScene::GetBlockByTile(int iX, int iY, int* pBx, int* pBy) {
    int iBx, iBy;
    OffsetToMap(iX, iY);
    if (iX < 0 || iY < 0)
        return NULL;
    iBx = iX / g_iSceneTileCntX;
    iBy = iY / g_iSceneTileCntY;
    if (iBx >= m_iBlockXCnt || iBy >= m_iBlockYCnt)
        return NULL;
    RtsSceneBlockMap* pBlockMap = &m_blockMapArray[iBy * m_iBlockXCnt + iBx];
    if (pBlockMap->GetState() != Object_State_Ready)
        return NULL;
    if (pBx)
        *pBx = iBx;
    if (pBy)
        *pBy = iBy;
    return (RtSceneBlock*)pBlockMap->m_pBlock;
}

RtsSTileMap* RtScene::GetTileMap(int iX, int iY) {
    int           iBx, iBy;
    RtSceneBlock* pBlock = GetBlockByTile(iX, iY, &iBx, &iBy);
    if (pBlock == NULL)
        return NULL;
    return pBlock->m_pTileMapIdx +
           ((iY - (iBy - m_iCenterPosY) * g_iSceneTileCntY) * g_iSceneTileCntX +
            (iX - (iBx - m_iCenterPosX) * g_iSceneTileCntX));
}

void RtScene::SetTileMapGrass(int iX, int iY, int Density, WORD tid, RtgVertex2 szh,
                              float AspectW) {
    int iBx = 0;
    int iBy = 0;

    RtSceneBlock* pBlock = GetBlockByTile(iX, iY, &iBx, &iBy);

    if (!pBlock)
        return;

    int          tileId = ((iY - (iBy - m_iCenterPosY) * g_iSceneTileCntY) * g_iSceneTileCntX +
                  (iX - (iBx - m_iCenterPosX) * g_iSceneTileCntX));
    RtsSTileMap* pMap   = pBlock->m_pTileMapIdx + tileId;

    if (pMap) {
        ClearGrass(pMap);
        CreateGrass(pMap, pBlock, pBlock->m_pVertices + tileId * 4, tid, Density, szh.x, szh.y,
                    AspectW);

        pBlock->SetModifyFlag(true);
        m_bSceneModify = true;
    }
}

void RtScene::SetTileMapMtl(int iX, int iY, BYTE cMtl, DWORD dwFlag) {
    int           iBx, iBy;
    RtSceneBlock* pBlock = GetBlockByTile(iX, iY, &iBx, &iBy);
    if (pBlock == NULL)
        return;
    RtsSTileMap* pMap = pBlock->m_pTileMapIdx +
                        ((iY - (iBy - m_iCenterPosY) * g_iSceneTileCntY) * g_iSceneTileCntX +
                         (iX - (iBx - m_iCenterPosX) * g_iSceneTileCntX));
    if (pMap) {
        pBlock->SetModifyFlag(true);
        if (dwFlag & 0x01)
            pMap->mtl[0] = cMtl;
        if (dwFlag & 0x02)
            pMap->mtl[1] = cMtl;
        if (dwFlag & 0x04)
            pMap->mtl[2] = cMtl;
        if (dwFlag & 0x08)
            pMap->mtl[3] = cMtl;
        GetSortTileMapIdx(*pMap);
        m_bSceneModify = true;
    }
}

void RtScene::SetTileWaterType(int iX, int iY, BYTE cWaterType) {
    int           iBx, iBy;
    RtSceneBlock* pBlock = GetBlockByTile(iX, iY, &iBx, &iBy);
    if (pBlock == NULL)
        return;
    RtsSTileMap* pMap = pBlock->m_pTileMapIdx +
                        ((iY - (iBy - m_iCenterPosY) * g_iSceneTileCntY) * g_iSceneTileCntX +
                         (iX - (iBx - m_iCenterPosX) * g_iSceneTileCntX));
    if (pMap) {
        if (pMap->cWaterType != cWaterType) {
            pBlock->SetModifyFlag(true);
            pMap->cWaterType = cWaterType;
            m_bSceneModify   = true;
        }
    }
}

RtSceneBlock* RtScene::GetBlock(int iX, int iY) {
    RtsSceneBlockMap* pMap = GetBlockMap(iX, iY);
    if (pMap == NULL)
        return NULL;
    return (RtSceneBlock*)pMap->m_pBlock;
}

void RtScene::ComputeVertexColor(int iX, int iY) {
    int           iBx, iBy;
    RtSceneBlock* pBlock = GetBlockByTile(iX, iY, &iBx, &iBy);
    if (pBlock) {
        pBlock->ComputeVertexColor(iX + (m_iCenterPosX - iBx) * g_iSceneTileCntX,
                                   iY + (m_iCenterPosY - iBy) * g_iSceneTileCntY);
    }
}

/************************************************************************
************************************************************************/
void RtScene::ComputeVertexNormal(int iX, int iY) {
    RtsSVertex* pV;
    RtgVertex3  l1, l2, l3, l4, l5, l6;
    RtgVertex3  n1, n2, n3, n4, n5, n6;

    pV = GetBlockVertex(iX - 1, iY - 1);
    if (pV == NULL)
        return;
    l1 = pV[3].pPos->v - pV[2].pPos->v;
    l2 = pV[3].pPos->v - pV[1].pPos->v;
    pV = GetBlockVertex(iX, iY - 1);
    if (pV == NULL)
        return;
    l3 = pV[2].pPos->v - pV[1].pPos->v;
    l4 = pV[2].pPos->v - pV[3].pPos->v;
    pV = GetBlockVertex(iX - 1, iY);
    if (pV == NULL)
        return;
    l5 = pV[1].pPos->v - pV[3].pPos->v;
    l6 = pV[1].pPos->v - pV[2].pPos->v;

    n1 = l1.Cross(l2);
    n2 = l2.Cross(l3);
    n3 = l3.Cross(l4);
    n4 = l4.Cross(l5);
    n5 = l5.Cross(l6);
    n6 = l6.Cross(l1);

    n1 = (n1 + n2 + n3 + n4 + n5 + n6) / 6.f;
    n1.Normalize();
    pV = GetBlockVertex(iX, iY);
    if (pV == NULL)
        return;
    pV->pPos->n = n1;
}

bool RtScene::AddObject(objectId _objId) {
    RtsSGrid*   pGrid = NULL;
    RtgVertex3  extB;
    RtgMatrix16 ma, mb;
    int         iMinX      = 0;
    int         iMinY      = 0;
    int         iMaxX      = 0;
    int         iMaxY      = 0;
    float       fGridWidth = g_fSceneTileWidth * RTS_BLOCK_GRID_SIZE;
    RtsObject*  pObject    = RtObjectManager()->GetObject(_objId);

    if (!pObject)
        return false;

    pObject->GetGridRange(iMinX, iMinY, iMaxX, iMaxY);

    ma = pObject->GetMatrix();
    ma.Translate(pObject->Get_AABBox()->vPos.x, pObject->Get_AABBox()->vPos.y,
                 pObject->Get_AABBox()->vPos.z);
    mb.Unit();
    extB.y = extB.x = fGridWidth * 0.5f;
    extB.z          = g_fSceneHeightStep * 1000;

    RtgVertex3 posA, posB, extA;
    RtgVertex3 ar[3], br[3];
    posB.z = 0.f;
    extA   = pObject->Get_AABBox()->vExt;
    if (pObject->IsKindOf(RT_RUNTIME_CLASS(RtsModel))) {
        float fScale = ((RtgVertex3*)&(ma._00))->Length();
        extA *= fScale;
    }
    ma.GetRow(3, posA);
    for (int i = 0; i < 3; i++) {
        ma.GetRow(i, ar[i]);
        ar[i].Normalize();
        mb.GetRow(i, br[i]);
        br[i].Normalize();
    }

    for (int y = iMinY; y <= iMaxY; y++) {
        for (int x = iMinX; x <= iMaxX; x++) {
            pGrid = GetGrid(x, y, true);

            if (pGrid == NULL)
                continue;

            posB.x = (pGrid->vMax.x + pGrid->vMin.x) * 0.5f;
            posB.y = (pGrid->vMax.y + pGrid->vMin.y) * 0.5f;

            if (rtgIsOBBIntersectOBB(extA, posA, ar, extB, posB, br)) {
                pGrid->aryObject.resize(pGrid->aryObject.size() + 1);
                pGrid->aryObject.back() = _objId;
                ++(pObject->m_uRef);
                pGrid->pBlock->SetModifyFlag(true);

                if (pGrid->vMax.z < ma._32)
                    pGrid->vMax.z = ma._32;
            }
        }
    }

    m_bSceneModify = true;

    return true;
}

bool RtScene::RemoveObject(objectId _objId) {
    RtsSGrid*   pGrid;
    RtgVertex3  b;
    RtgMatrix16 ma, mb;
    int         iMinX      = 0;
    int         iMinY      = 0;
    int         iMaxX      = 0;
    int         iMaxY      = 0;
    int         i          = 0;
    float       fGridWidth = g_fSceneTileWidth * RTS_BLOCK_GRID_SIZE;

    for (int y = iMinY; y <= iMaxY; y++) {
        for (int x = iMinX; x <= iMaxX; x++) {
            pGrid = GetGrid(x, y, true);

            if (pGrid == NULL)
                continue;

            for (i = 0; i < pGrid->aryObject.size(); i++)
                if (pGrid->aryObject[i] == _objId)
                    break;

            if (i != pGrid->aryObject.size())
                pGrid->aryObject.swap_erase(i);
        }
    }

    m_bSceneModify = true;

    return true;
}

BOOL RtScene::AddDynamicLightToScene(RtsDynamicLight* pLight) {
    int          iGx, iGy;
    RtsSGrid*    pGrid;
    RtgMatrix16& mObj = pLight->GetMatrix();
    if (!GetGridByPos(mObj._30, mObj._31, iGx, iGy)) {
        return TRUE;  // 没有找到网格需要添加
    }
    pGrid = GetGrid(iGx, iGy, false);
    if (pGrid) {
        pGrid->dLight.push_back(pLight);
        if (pGrid->dLight.size() > 10) {
            RtCoreLog().Error("RtScene:同一个网格内的动态灯光太多 [%d]，会被删除10个\n",
                              pGrid->dLight.size());
            for (iGx = 0; iGx < 10; iGx++) {
                pGrid->dLight.pop_front();
            }
        }
    }
    return TRUE;
}

BOOL RtScene::RemoveDynamicLightByScene(RtsDynamicLight* pLight) {
    int          iGx, iGy;
    RtsSGrid*    pGrid;
    RtgMatrix16& mObj = pLight->GetMatrix();
    if (!GetGridByPos(mObj._30, mObj._31, iGx, iGy)) {
        return TRUE;  // 没有找到网格
    }
    pGrid = GetGrid(iGx, iGy, false);
    if (pGrid) {
        pGrid->dLight.remove(pLight);
    }
    return TRUE;
}

RtsLight* RtScene::GetRangeLights(RtgVertex3& vPos, float fRadius, BYTE cFlag) {
    struct _LightHolder {
        RtsLight* _light;

        _LightHolder(RtsLight* _lit) : _light(_lit) {}

        bool operator<(const _LightHolder& _lh) const {
            return _light->fDistance < _lh._light->fDistance;
        }
    };

    set<_LightHolder> setlh;

    RtsObject*    pObj;
    RtsSGrid*     pGrid;
    RtsLight *    pFirstLight, *pIt;
    RtgLightItem* pLights;
    int           iGx, iGy, iMinX, iMinY, iMaxX, iMaxY, x, y, i;
    bool          bSLight = (cFlag & 0x01) != 0;
    bool          bDLight = (cFlag & 0x02) != 0;
    this->GetGridByPos(vPos, iGx, iGy);
    i           = fRadius / (g_fSceneTileWidth * RTS_BLOCK_GRID_SIZE);
    iMinX       = iGx - i;
    iMinY       = iGy - i;
    iMaxX       = iGx + i;
    iMaxY       = iGy + i;
    pLights     = NULL;
    pFirstLight = NULL;

    for (y = iMinY; y <= iMaxY; y++) {
        for (x = iMinX; x <= iMaxX; x++) {
            pGrid = GetGrid(x, y, false);
            if (pGrid == NULL)
                continue;
            if (bSLight && pGrid->aryObject.size()) {
                for (i = 0; i < pGrid->aryObject.size(); i++) {
                    pObj = RtObjectManager()->GetObject(pGrid->aryObject[i]);

                    if (pObj && pObj->IsKindOf(RT_RUNTIME_CLASS(RtsLight))) {
                        pIt = (RtsLight*)pObj;
                        pIt->fDistance =
                            vPos.Distance2(*((RtgVertex3*)(pIt->GetMatrix().Position())));
                        if (pIt->fDistance <= (fRadius + pIt->fRange) * (fRadius + pIt->fRange)) {
                            setlh.insert(pIt);
                        }
                    }
                }
            }

            if (bDLight && pGrid->dLight.size()) {
                list<RtsDynamicLight*>::iterator it;
                for (it = pGrid->dLight.begin(); it != pGrid->dLight.end(); it++) {
                    pIt = (*it);
                    // 以下一段代码复制自上面，请不要修改，修改上面的时候请更新这个
                    pIt->fDistance = vPos.Distance2(*((RtgVertex3*)(pIt->GetMatrix().Position())));
                    if (pIt->fDistance <= (fRadius + pIt->fRange) * (fRadius + pIt->fRange)) {

                        setlh.insert(pIt);
                    }
                }
            }
        }
    }

    if (setlh.empty())
        return NULL;

    set<_LightHolder>::iterator _it = setlh.begin();
    pFirstLight                     = (*_it)._light;
    pIt                             = pFirstLight;

    while (++_it != setlh.end()) {
        pIt->pNext = (*_it)._light;
        pIt        = (RtsLight*)pIt->pNext;
    }

    pIt->pNext = NULL;

    return pFirstLight;
}

bool RtScene::TakeGeometry(const RtgAABB& inAABB, RtgVertex3* pVertices, int& iVertexNum,
                           short* pIndices, int& iIndexNum) {

    int               iBMinX, iBMinY, iBMaxX, iBMaxY, bx, by;
    int               iTMinX, iTMinY, iTMaxX, iTMaxY;
    int               iMinX, iMinY, iMaxX, iMaxY, x, y;
    int               iVB, iIB, iCnt;
    short             pIdx[6] = {0, 1, 2, 1, 3, 2};
    float             fPosMinX, fPosMinY, fPosMaxX, fPosMaxY;
    RtsSceneBlockMap* pBlockMap;
    RtSceneBlock*     pBlock;

    fPosMinX = inAABB.vPos.x - inAABB.vExt.x;
    fPosMinY = inAABB.vPos.y - inAABB.vExt.y;
    OffsetToMap(fPosMinX, fPosMinY);
    iBMinX = (int)(fPosMinX / (g_fSceneBlockWidth));
    iBMinY = (int)(fPosMinY / (g_fSceneBlockHeight));
    if (iBMinX >= m_iBlockXCnt)
        iBMinX = m_iBlockXCnt - 1;
    if (iBMinY >= m_iBlockYCnt)
        iBMinY = m_iBlockYCnt - 1;

    fPosMaxX = inAABB.vPos.x + inAABB.vExt.x;
    fPosMaxY = inAABB.vPos.y + inAABB.vExt.y;
    OffsetToMap(fPosMaxX, fPosMaxY);
    iBMaxX = (int)(fPosMaxX / (g_fSceneBlockWidth));
    iBMaxY = (int)(fPosMaxY / (g_fSceneBlockHeight));
    if (iBMaxX >= m_iBlockXCnt)
        iBMaxX = m_iBlockXCnt - 1;
    if (iBMaxY >= m_iBlockYCnt)
        iBMaxY = m_iBlockYCnt - 1;

    iTMinX = (int)(fPosMinX / g_fSceneTileWidth);
    iTMinY = (int)(fPosMinY / g_fSceneTileWidth);
    iTMaxX = (int)(fPosMaxX / g_fSceneTileWidth);
    iTMaxY = (int)(fPosMaxY / g_fSceneTileWidth);

    iCnt = 0;
    RtsSVertex* pPos;
    for (by = iBMinY; by <= iBMaxY; by++) {
        for (bx = iBMinX; bx <= iBMaxX; bx++) {
            pBlockMap = &m_blockMapArray[by * m_iBlockXCnt + bx];

            if (pBlockMap->GetState() != Object_State_Ready)
                continue;

            pBlock = (RtSceneBlock*)pBlockMap->m_pBlock;
            RTASSERT(pBlock);

            iMinX = bx * g_iSceneTileCntX;
            iMinY = by * g_iSceneTileCntY;
            iMaxX = iMinX + g_iSceneTileCntX - 1;
            iMaxY = iMinY + g_iSceneTileCntY - 1;

            if (iMinX < iTMinX)
                iMinX = iTMinX;
            if (iMinY < iTMinY)
                iMinY = iTMinY;
            if (iMaxX >= iTMaxX)
                iMaxX = iTMaxX;
            if (iMaxY >= iTMaxY)
                iMaxY = iTMaxY;

            iMinX -= g_iSceneTileCntX * bx;
            iMinY -= g_iSceneTileCntY * by;
            iMaxX -= g_iSceneTileCntX * bx;
            iMaxY -= g_iSceneTileCntY * by;

            for (y = iMinY; y <= iMaxY; y++) {
                for (x = iMinX; x <= iMaxX; x++) {
                    pPos = pBlock->m_pVertices + (y * g_iSceneTileCntX + x) * 4;

                    if (pVertices) {
                        pVertices[0].x = pPos[0].pPos->v.x;
                        pVertices[0].y = pPos[0].pPos->v.y;
                        pVertices[0].z = pPos[0].pPos->fTerrainHeight;
                        pVertices[1].x = pPos[1].pPos->v.x;
                        pVertices[1].y = pPos[1].pPos->v.y;
                        pVertices[1].z = pPos[1].pPos->fTerrainHeight;
                        pVertices[2].x = pPos[2].pPos->v.x;
                        pVertices[2].y = pPos[2].pPos->v.y;
                        pVertices[2].z = pPos[2].pPos->fTerrainHeight;
                        pVertices[3].x = pPos[3].pPos->v.x;
                        pVertices[3].y = pPos[3].pPos->v.y;
                        pVertices[3].z = pPos[3].pPos->fTerrainHeight;
                        pVertices += 4;
                    }

                    if (pIndices) {
                        pIndices[0] = iCnt + pIdx[0];
                        pIndices[1] = iCnt + pIdx[1];
                        pIndices[2] = iCnt + pIdx[2];
                        pIndices[3] = iCnt + pIdx[3];
                        pIndices[4] = iCnt + pIdx[4];
                        pIndices[5] = iCnt + pIdx[5];
                        pIndices += 6;
                    }

                    iCnt += 4;
                }
            }
        }
    }

    iVB = iCnt;
    iIB = iVB + (iCnt >> 1);

    if (pVertices && pIndices) {
        CHECK(iVertexNum >= iVB && iIndexNum >= iIB);
    }

    iVertexNum = iVB;
    iIndexNum  = iIB;

    return true;
}

#if DO_CONSOLE
void RtScene::BeginPickRayRecord(RtgVertex3& vSource, RtgVertex3& vDir) {
    m_vPickRaySource       = vSource;
    m_vPickRayDir          = vDir;
    m_bEnablePickRayRecord = TRUE;
    m_listPickRayLine.clear();
    m_listPickRayPoint.clear();
}

void RtScene::EndPickRayRecord() {
    m_bEnablePickRayRecord = FALSE;
}

void RtScene::AddPickRayLine(RtgVertex3& v1, RtgVertex3& v2) {
    m_listPickRayLine.push_back(v1);
    m_listPickRayLine.push_back(v2);
}

void RtScene::AddPickRayPoint(float fDist) {
    m_listPickRayPoint.push_back(fDist);
}

void RtScene::DrawPickRayRecord() {
    if (m_listPickRayLine.size() == 0)
        return;
    RtgShader  nullShader;
    RtgVertex3 v1, v2;
    DWORD      dwColor[3] = {0xFFFF0000, 0xFF0000FF, 0xFF00FF00};
    m_pDevice->SetShader(&nullShader);
    DWORD dwLighting = m_pDevice->GetRenderState(RTGRS_LIGHTING);
    m_pDevice->SetRenderState(RTGRS_LIGHTING, 0);
    int                        iFlag = 0;
    list<RtgVertex3>::iterator it    = m_listPickRayLine.begin();
    for (; it != m_listPickRayLine.end(); it++) {
        v1 = *it;
        it++;
        v2 = *it;
        m_pDevice->DrawLine(v1, v2, dwColor + iFlag);
        iFlag = (iFlag + 1) % 3;
    }
    m_pDevice->SetRenderState(RTGRS_LIGHTING, dwLighting);
    m_pDevice->RestoreShader(&nullShader);
}
#endif

RtsModel* RtScene::FindFirstModel(const char* szFilename) {
    int       ix, iy;
    RtsModel* pModel;
    for (iy = 0; iy < m_iBlockYCnt; iy++) {
        for (ix = 0; ix < m_iBlockXCnt; ix++) {
            if (m_blockMapArray[iy * m_iBlockXCnt + ix].m_pBlock) {
                pModel = ((RtSceneBlock*)m_blockMapArray[iy * m_iBlockXCnt + ix].m_pBlock)
                             ->FindFirstModel(szFilename);
                if (pModel)
                    return pModel;
            }
        }
    }
    return NULL;
}

void RtScene::RecomputeAllLight() {
    LoadAllBlock();
    RtSceneBlock* pBlock = GetFirstBlock();
    while (pBlock) {
        pBlock->RecomputeAllLight(*m_pDevice);
        pBlock = GetNextBlock(pBlock);
    }
}

void RtScene::RecomputeObjectBelong() {
    RTASSERT(false);
    //   m_ObjectManger.RecomputeObjectBelong(m_pDevice);
}

// 检查场景中完全相同的物体(位置和物件都相同)
int RtScene::CheckSameObject() {
    RTASSERT(false);
    return 0;
    //return m_ObjectManger.CheckSameObject();
}

// 删除场景中完全相同的物体(位置和物件都相同)
int RtScene::DeleteSameObject() {
    RTASSERT(false);
    return 0;
    // return m_ObjectManger.DeleteSameObject();
}

//void RtScene::GetRenderObject(RtsObject**& plistObj, int& nNum)
//{
//    RTASSERT(plistObj);
//    RTASSERT(nNum);
//
//    plistObj = m_ObjectRender.m_apObject;
//    nNum = m_ObjectRender.m_objNum;
//}

bool RtScene::DeleteBlockMap(int x, int y) {
    int iPosX = x + m_iCenterPosX;
    int iPosY = y + m_iCenterPosY;

    if (!(iPosX >= 0 && iPosY >= 0 && iPosX < m_iBlockXCnt && iPosY < m_iBlockYCnt))
        return false;

    if (!(*(m_blockMapArray[iPosY * m_iBlockXCnt + iPosX].m_szFileName)))
        return false;

    m_blockMapArray[iPosY * m_iBlockXCnt + iPosX].SetFileName("");
    m_iBlockCnt--;
    m_SceneBlockManager.ReleaseSceneBlock(&m_blockMapArray[iPosY * m_iBlockXCnt + iPosX]);

    return true;
}

void RtScene::SetRenderMode(BOOL bMiniMap) {
    if (m_bRenderMiniMapMode == bMiniMap)
        return;
    CHECK((!m_bRenderMiniMapMode) == m_pDevice->m_pCamera->IsPerspective());
    m_bRenderMiniMapMode = bMiniMap;
    if (m_bRenderMiniMapMode) {
        m_pDevice->m_pCamera->SaveToBuffer(m_pCameraSaveBuffer);
        m_pDevice->m_pCamera->SetPitch(0.f);
        m_pDevice->m_pCamera->SetRoll(0.f);
        m_pDevice->m_pCamera->SetYaw(0.f);
        m_pDevice->m_pCamera->ToggleProjectType();
        SetSkyLighting(0xFF808080);
        m_lightDirect.vDiffuse.Set(1.0f);
    } else {
        m_pDevice->m_pCamera->ToggleProjectType();
        m_pDevice->m_pCamera->LoadFromBuffer(m_pCameraSaveBuffer);
    }
    m_pDevice->m_pCamera->ReUpdate();
}

//
//RtgLight* RtScene::CreateDynamicLight()
//{
//    RtsDynamicLight* pLight = (RtsDynamicLight*)m_ObjectManager.NewObject(RT_RUNTIME_CLASS(RtsDynamicLight));
//    return pLight;
//}
//
//void RtScene::DestroyDynamicLight(RtgLight* pLight)
//{
//    RemoveDynamicLightByScene((RtsDynamicLight*)pLight);
//    RtsObject::Destroy((RtsDynamicLight*)pLight);
//}

//void RtScene::UpdateDynamicLight(RtgLight* pLight)
//{
//    RtgMatrix16& vm = ((RtsDynamicLight*)pLight)->GetMatrix();
//    if (vm._30!=pLight->vPosition.x || vm._31!=pLight->vPosition.y || vm._32!=pLight->vPosition.z)
//    {
//        RemoveDynamicLightByScene((RtsDynamicLight*)pLight);
//        RtgMatrix16 m16;
//        m16.Unit();
//        m16._30 = pLight->vPosition.x;
//        m16._31 = pLight->vPosition.y;
//        m16._32 = pLight->vPosition.z;
//        ((RtsDynamicLight*)pLight)->SetMatrix(m16);
//        AddDynamicLightToScene((RtsDynamicLight*)pLight);
//    }
//}

void SceneMiniMapRotate2(int centerX, int centerY, int inX, int inY, int& outX, int& outY, float fc,
                         float fs) {
    inX -= centerX;
    inY -= centerY;
    outX = (int)(inX * fc - inY * fs) + centerX;
    outY = (int)(inX * fs + inY * fc) + centerY;
}

void SceneMiniMapRotate(int centerX, int centerY, int inX, int inY, int& outX, int& outY,
                        float fRadian) {
    float fc = fcos(fRadian);
    float fs = fsin(fRadian);
    inX -= centerX;
    inY -= centerY;
    outX = (int)(inX * fc - inY * fs) + centerX;
    outY = (int)(inX * fs + inY * fc) + centerY;
}

void RtScene::DrawMiniMap(int iScreenX, int iScreenY, int iScreenWidth, int iScreenHeight,
                          float fSceneWidth, float fCenterX, float fCenterY) {
    RtgNVertexVCT2 p[4];
    float          fX, fY, fTexTx, fTexTy, fTexBx, fTexBy;
    int            iBx, iBy, iX, iY, iSx, iSy, iEx, iEy;  //Block行 Block列
    int iDltx, iDlty, iDlbx, iDlby, iDrtx, iDrty, iDrbx, iDrby;  //小地图在屏幕坐标系的4个顶点坐标
    float fMinX, fMinY, fMaxX, fMaxY, fMinXO, fMinYO, fSceneHeight;

    float tl, tr, tb, tt, t2l, t2r, t2b, t2t;  //UV坐标
    DWORD dwColor       = 0xFFFFFFFF;
    float fMapRotateCos = fcos(m_fMinimapRotate);
    float fMapRotateSin = fsin(m_fMinimapRotate);

    const float fEMin = 0.002f, fEMax = 0.998f;
    int         Quadrant = 0;

    OffsetToMap(fCenterX, fCenterY);

    // 按照比率取出需要显示的高度
    fSceneHeight = fSceneWidth * iScreenHeight / iScreenWidth;

    // 计算出周围四个点的位置，并且察看分割方案，同时计算出分割线位置

    fMinX = fCenterX - fSceneWidth / 2.f;
    fMaxX = fCenterX + fSceneWidth / 2.f;
    fMinY = fCenterY - fSceneHeight / 2.f;
    fMaxY = fCenterY + fSceneHeight / 2.f;

    int iCenterX = iScreenX + iScreenWidth * 0.5f;
    int iCenterY = iScreenY + iScreenHeight * 0.5f;  //屏幕中心坐标

    fMinXO = fMinX;
    fMinYO = fMinY;

    iSx = iScreenX;
    iSy = iScreenY + iScreenHeight;  //屏幕左下角坐标

    if (fMinX < 0.f) {
        iSx += iScreenWidth * (0.f - fMinX) / fSceneWidth;
        fMinX = 0.f;
    } else if (fMaxX > g_fSceneBlockWidth * GetBlockCntX()) {
        fMaxX = g_fSceneBlockWidth * GetBlockCntX();
    }

    if (fMinY < 0.f) {
        iSy -= iScreenHeight * (0.f - fMinY) / fSceneHeight;
        fMinY = 0.f;
    } else if (fMaxY > g_fSceneBlockHeight * GetBlockCntY()) {
        fMaxY = g_fSceneBlockHeight * GetBlockCntY();
    }

    fY  = fMinY;
    iY  = iSy;
    iBy = (int)(fY / m_fMinimapHeight);

    while (fY < fMaxY) {
        fTexBy = m_fMinimapHeight * iBy;
        fTexTy = fTexBy + m_fMinimapHeight;

        if (fTexTy > fMaxY)
            fTexTy = fMaxY;

        tt  = 1 - (fTexTy - fTexBy) / m_fMinimapHeight;
        tb  = 1 - (fY - fTexBy) / m_fMinimapHeight;
        t2t = (fTexTy - fMinYO) / fSceneHeight;
        t2t = Clamp(t2t, 0.f, 1.f);
        t2t = t2t * (m_fMinimapMaskBottom - m_fMinimapMaskTop) + (1.f - m_fMinimapMaskBottom);
        t2b = (fTexBy - fMinYO) / fSceneHeight;
        t2b = Clamp(t2b, 0.f, 1.f);
        t2b = t2b * (m_fMinimapMaskBottom - m_fMinimapMaskTop) + (1.f - m_fMinimapMaskBottom);
        tt  = Clamp(tt, fEMin, fEMax);
        tb  = Clamp(tb, fEMin, fEMax);
        t2t = Clamp(t2t, fEMin, fEMax);
        t2b = Clamp(t2b, fEMin, fEMax);
        iEy = iY - iScreenHeight * (fTexTy - fY) / fSceneHeight;

        fX  = fMinX;
        iX  = iSx;
        iBx = (int)(fX / m_fMinimapWidth);
        while (fX < fMaxX) {
            fTexTx = m_fMinimapWidth * iBx;
            fTexBx = fTexTx + m_fMinimapWidth;

            if (fTexBx > fMaxX)
                fTexBx = fMaxX;

            RtsSceneBlockMap* pMap    = GetBlockMap(iBx / 2 - GetCenterX(), iBy / 2 - GetCenterY());
            RtgTextItem*      texItem = NULL;

            if (pMap && pMap->GetState() == Object_State_Ready)
                texItem = ((RtSceneBlock*)pMap->m_pBlock)->m_texMinmap[(iBy % 2) * 2 + (iBx % 2)];

            iEx = iX + iScreenWidth * (fTexBx - fX) / fSceneWidth;

            if (texItem && texItem->state == Object_State_Ready) {
                tl  = (fX - fTexTx) / m_fMinimapWidth;
                tr  = (fTexBx - fTexTx) / m_fMinimapWidth;
                t2l = (fTexTx - fMinXO) / fSceneWidth;
                t2l = Clamp(t2l, 0.f, 1.f);
                t2l = t2l * (m_fMinimapMaskRight - m_fMinimapMaskLeft) + m_fMinimapMaskLeft;
                t2r = (fTexBx - fMinXO) / fSceneWidth;
                t2r = Clamp(t2r, 0.f, 1.f);
                t2r = t2r * (m_fMinimapMaskRight - m_fMinimapMaskLeft) + m_fMinimapMaskLeft;
                tl  = Clamp(tl, fEMin, fEMax);
                tr  = Clamp(tr, fEMin, fEMax);
                t2l = Clamp(t2l, fEMin, fEMax);
                t2r = Clamp(t2r, fEMin, fEMax);

                SceneMiniMapRotate2(iCenterX, iCenterY, iX, iEy, iDltx, iDlty, fMapRotateCos,
                                    fMapRotateSin);
                SceneMiniMapRotate2(iCenterX, iCenterY, iX, iY, iDlbx, iDlby, fMapRotateCos,
                                    fMapRotateSin);
                SceneMiniMapRotate2(iCenterX, iCenterY, iEx, iEy, iDrtx, iDrty, fMapRotateCos,
                                    fMapRotateSin);
                SceneMiniMapRotate2(iCenterX, iCenterY, iEx, iY, iDrbx, iDrby, fMapRotateCos,
                                    fMapRotateSin);

                p[0] = RtgNVertexVCT2(iDltx, iDlty, dwColor, tl, tt, t2l, t2t);
                p[1] = RtgNVertexVCT2(iDlbx, iDlby, dwColor, tl, tb, t2l, t2b);
                p[2] = RtgNVertexVCT2(iDrbx, iDrby, dwColor, tr, tb, t2r, t2b);
                p[3] = RtgNVertexVCT2(iDrtx, iDrty, dwColor, tr, tt, t2r, t2t);

                m_ShaderMinimap.Shaders[0].SetTexture(texItem);

                m_pDevice->SetShader(&m_ShaderMinimap);
                m_pDevice->SetVertexFormat(RTG_TRANSFORMED_VCT2);
                m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, &p, sizeof(RtgNVertexVCT2), 2);
                m_pDevice->RestoreShader(&m_ShaderMinimap);
            }

            fX = fTexBx;
            iX = iEx;
            iBx++;
        }
        fY = fTexTy;
        iY = iEy;
        iBy++;
    }
}

void RtScene::SetMinimapMaskTexture(const char* szTextureFileName, float fL, float fT, float fR,
                                    float fB)  // "minimap_mask.tga"
{
    RtString sNewMinimapMask = szTextureFileName;
    if (m_strMinimapMaskTexture != szTextureFileName) {
        m_strMinimapMaskTexture = szTextureFileName;
        if (m_ShaderMinimap.Shaders[1].GetTexture()) {
            RtGetRender()->GetTextMgr()->ReleaseTexture(m_ShaderMinimap.Shaders[1].GetTexture());
            m_ShaderMinimap.Shaders[1].SetTexture(0);
        }
        m_ShaderMinimap.Shaders[1].SetTexture(
            RtGetRender()->GetTextMgr()->CreateUiTexture(m_strMinimapMaskTexture));
        m_fMinimapMaskLeft   = fL;
        m_fMinimapMaskTop    = fT;
        m_fMinimapMaskRight  = fR;
        m_fMinimapMaskBottom = fB;
    }
}

//
//BOOL RtScene::AddObjToQueue(RtsObject* pObject)
//{
//    if(pObject->IsKindOf(RT_RUNTIME_CLASS(RtsModel)))
//    {
//        float size = pObject->m_AABB.vExt.Length();
//        float dis  = rtgDistance(m_pDevice->m_pCamera->m_vEyePt, pObject->GetMatrix().Position());
//        short rate = 100*dis/size;
//        pObject->SetLoading(true);
//        m_mapSceneObj.insert(make_pair(rate,(RtsModel*)pObject));
//        return true;
//    }
//    return false;
//}
//
//void RtScene::LoadObjInQueue()
//{
//    DWORD time = timeGetTime();
//    DWORD timeNow = 0;
//    multimap<short,RtsModel*>::iterator iter;
//    iter = m_mapSceneObj.begin();
//    while(iter != m_mapSceneObj.end())
//    {
//        RtsModel* pObj = iter->second;
//        if (!pObj->IsLoaded())
//        {
//            pObj->Reload(m_pDevice);
//            iter = m_mapSceneObj.erase(iter);
//            //RtCoreLog().Info("CONTINUE LOADING MODEL ::%i \n",m_mapSceneObj.size());
//        }
//        else
//        {
//            iter++;
//        }
//
//        timeNow = timeGetTime();
//
//        if (timeNow - time > 40)
//        {
//            //RtCoreLog().Info("LOADING TIME OUT \n");
//            return;
//        }
//
//    }
//}
/*------------------------------------------------------------------------
-   The End.
------------------------------------------------------------------------*/

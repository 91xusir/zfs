
//////////////////////////////////////////////////////////////////////////
//
//   FileName : rts_grass_render.h
//     Author : zxy
// CreateTime : 2009.12.20
//       Desc :
//
//////////////////////////////////////////////////////////////////////////

#ifndef _RtsGrassRender_H_
#define _RtsGrassRender_H_

struct RtsSTileMap;
struct RtsSGrid;
struct RtsSVertex;

#define _TexId(v) (((v) >> 8) & 0xff)
#define _SubIx(v) ((v) & 0xff)

class RtsGrassRender : public RtScriptXML
{
public :
    typedef list<RtsSTileMap*> tileList;

    typedef struct _RtgGrassText
    {
        string name;
        string file;
        int sub_x;
        int sub_y;
        RtgTextItem* texItem;

    } RtgGrassText;


    typedef struct _PassInfo 
    {
        tileList::iterator begin;
        tileList::iterator end;
        UINT uNumGrass;
    
    } PassInfo;

    enum 
    {
        Max_Grass_Pre_Pass = 5120,
        Max_Grass_Text_Id = 255
    };

public :
    bool Init(RtgDevice* pDev);
    void Render(RtgCamera* pCam);
    void Reset();
    void CalcGrassTextCoord(RtgVertex4* uv, WORD tId);

    bool LoadGrassTexture(const char* fileName);
    void ClearGrassTexture();
    void OnTagStart(const char* tag);
    void OnAttribute(const char* atr, const char* val);
    void OnTagEnd();
    void OnTagEndImme() { OnTagEnd(); }

    RtsGrassRender();
    virtual ~RtsGrassRender();

    inline void AddGrass(RtsSTileMap* tile){
        m_mapGrass[_TexId(tile->grassId)].push_back(tile);
    }

public :
    map<UINT, tileList>  m_mapGrass;
    RtgGrassText         m_curText;
    int                  m_curIdx;
    RtgGrassText*        m_apText[Max_Grass_Text_Id];
    RtgShader            m_Shader;
    RtgDevice*           m_pDevice;


};


#endif


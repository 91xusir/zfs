
//////////////////////////////////////////////////////////////////////////
//
//   FileName : rtg_texture_util.cpp
//     Author : zxy
// CreateTime : 2010.03.12
//       Desc :
//
//////////////////////////////////////////////////////////////////////////

#include "graph\rtg_texture_util.h"
#include "graph\rt_graph.h"

namespace rt_graph {

    RtgTextItem* TextUtil_CreateChessboard()
    {
        const int _ChessSizeX = 1024;
        const int _ChessSizeY = 512;
        const int _GridSize  = 32;
        const int _GridNumX   = _ChessSizeX / _GridSize;
        const int _GridNumY   = _ChessSizeY / _GridSize;
        const int _ColorSize  = _ChessSizeX * _ChessSizeY * sizeof(DWORD);

        DWORD* _Color = (DWORD*)RT_NEW char[_ColorSize];
        memset(_Color, 0, _ColorSize);

        for (int y = 0; y < _GridNumY; ++y)
        {
            int xs = (y % 2) ? 0 : 1;
            for (int x = xs; x < _GridNumX; x += 2)
            {
                int y0 = y * _GridSize;
                int y1 = y0 + _GridSize;
                int x0 = x * _GridSize;
                int x1 = x0 + _GridSize;

                for (int j = y0; j < y1; ++j)
                    for (int i = x0; i < x1; ++i)
                        _Color[j * _ChessSizeX + i] = 0xffffffff;

            }
        }

        RtgTextItem* texItem = RtGetRender()->GetTextMgr()->CreateTextureFromARGBData(
            _Color, 
            _ChessSizeX * _ChessSizeY * sizeof(DWORD),
            _ChessSizeX,
            _ChessSizeY
            );

        DEL_ARRAY(_Color);

        return texItem;
    }

}


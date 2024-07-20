#include "region.h"
#include "region_spawn.h"
#include "region_ai.h"
#include "region_state_snapshot.h"

#ifndef _WIN32

typedef struct tagRGBQUAD {
	BYTE    rgbBlue;
	BYTE    rgbGreen;
	BYTE    rgbRed;
	BYTE    rgbReserved;
} RGBQUAD;

typedef struct tagBITMAPINFOHEADER{
	DWORD      biSize;
	LONG       biWidth;
	LONG       biHeight;
	WORD       biPlanes;
	WORD       biBitCount;
	DWORD      biCompression;
	DWORD      biSizeImage;
	LONG       biXPelsPerMeter;
	LONG       biYPelsPerMeter;
	DWORD      biClrUsed;
	DWORD      biClrImportant;
} BITMAPINFOHEADER;

typedef struct tagBITMAPINFO {
	BITMAPINFOHEADER    bmiHeader;
	RGBQUAD             bmiColors[1];
} BITMAPINFO;

typedef struct tagBITMAPFILEHEADER {
	WORD    bfType;
	DWORD   bfSize;
	WORD    bfReserved1;
	WORD    bfReserved2;
	DWORD   bfOffBits;
} BITMAPFILEHEADER;

#endif // _WIN32

CRegionStateSnapshot g_stateSnapshot;

void DrawRect(unsigned char* buf, int width, int height, int x, int y, int w, int h, int r, int g, int b)
{
	unsigned char* p = buf + width * y * 3 + x * 3;

	for (int ty = 0; ty < h; ty++)
	{
		for (int tx = 0; tx < w; tx++)
		{
			float val;

			val = p[tx * 3 + 0] / 255.0f * 0.9f + r / 255.0f * 0.1f;
			if (val > 1) val = 1;
			p[tx * 3 + 0] = (unsigned char)(255.0f * val + 0.0001f);

			val = p[tx * 3 + 1] / 255.0f * 0.9f + g / 255.0f * 0.1f;
			if (val > 1) val = 1;
			p[tx * 3 + 1] = (unsigned char)(255.0f * val + 0.0001f);

			val = p[tx * 3 + 2] / 255.0f * 0.9f + b / 255.0f * 0.1f;
			if (val > 1) val = 1;
			p[tx * 3 + 2] = (unsigned char)(255.0f * val + 0.0001f);
		}

		p += width * 3;
	}
}

void CRegionStateSnapshot::BuildSpawnMap(const char* filename)
{
#ifdef WIN32	
	unordered_map<CRegionSpawn*, RGBQUAD> colors;
	char lastcolor = 1;

	int cx = g_region->m_defScene->m_pTerrain->GetBlockCntX() * g_iSceneTerrainCntX;
	int cy = g_region->m_defScene->m_pTerrain->GetBlockCntY() * g_iSceneTerrainCntY;

	unsigned char* buf = RT_NEW unsigned char[cx * cy * 3];

	memset(buf, 0, cx * cy * 3);

	for (int y = 0; y < cy; y++)
	{
		for (int x = 0; x < cx; x++)
		{
			int tx = x;
			int ty = y;

			g_region->m_defScene->m_pTerrain->OffsetTerrainToWorld(tx, ty);

			const RtSceneBlockTerrain::STileAttr* attr = g_region->m_defScene->m_pTerrain->GetTerrainAttr(tx, ty);

            if(attr && attr->cAttr)
			{
				buf[y * cx * 3 + x * 3 + 0] = 0x3f;
				buf[y * cx * 3 + x * 3 + 1] = 0x3f;
				buf[y * cx * 3 + x * 3 + 2] = 0x3f;
			}
		}
	}

	stlforeach(list<CRegionSpawn>, spawn, g_spawnMgr.m_spawns)
	{
		int tx1, ty1, tx2, ty2;		

		g_region->m_defScene->m_pTerrain->GetTerrainByPosFast(spawn->m_areaBegin[0], spawn->m_areaBegin[1], tx1, ty1);	

		g_region->m_defScene->m_pTerrain->OffsetTerrainToMap(tx1, ty1);

		g_region->m_defScene->m_pTerrain->GetTerrainByPosFast(spawn->m_areaEnd[0], spawn->m_areaEnd[1], tx2, ty2);	

		g_region->m_defScene->m_pTerrain->OffsetTerrainToMap(tx2, ty2);

		DrawRect(buf, cx, cy, tx1, ty1, tx2 - tx1, ty2 - ty1, 0xff, 0xff, 0xff);
	}

	TObjectHash& objs = g_region->m_defScene->m_objectHash;

	RGBQUAD c;

	for (TObjectHash::iterator iter = objs.begin(); iter != objs.end(); iter++)
	{
		CRegionObject* obj = iter->second;

		if( (OB_TYPE(obj) != OBJECT_TYPE_CREATURE) &&
//-----------------add start by tony 05.05.19--------------------------------//
			(OB_TYPE(obj) != OBJECT_TYPE_NPC_COMBATIVE))
//-----------------add end   by tony 05.05.19--------------------------------//
			continue;

		CRegionCreature* cre = (CRegionCreature*)obj;

		if (!cre->IsMonster())
			continue;

		if (!cre->m_spawn)
			continue;

		if (colors.find(cre->m_spawn) == colors.end())
		{
			c.rgbRed = rand() % 127 + 127;
			c.rgbGreen = rand() % 127 + 64;
			c.rgbBlue = rand() % 64 + 64;
			colors[cre->m_spawn] = c;
		}
		else
		{
			c = colors[cre->m_spawn];
		}

		int fx = cre->m_pos[0];
		int fy = cre->m_pos[1];
		int tx, ty;

		g_region->m_defScene->m_pTerrain->GetTerrainByPosFast(fx, fy, tx, ty);	

		g_region->m_defScene->m_pTerrain->OffsetTerrainToMap(tx, ty);

		buf[ty * cx * 3 + tx * 3 + 0] = c.rgbRed;
		buf[ty * cx * 3 + tx * 3 + 1] = c.rgbGreen;
		buf[ty * cx * 3 + tx * 3 + 2] = c.rgbBlue;
	}

	SaveBitmap(filename, buf, cx, cy);

	DEL_ONE(buf);
#endif
}

void CRegionStateSnapshot::BuildTerrainMap(const char* filename, int mask)
{
	int cx = g_region->m_defScene->m_pTerrain->GetBlockCntX() * g_iSceneTerrainCntX;
	int cy = g_region->m_defScene->m_pTerrain->GetBlockCntY() * g_iSceneTerrainCntY;

	unsigned char* buf = RT_NEW unsigned char[cx * cy];

	memset(buf, 0, cx * cy);

	for (int y = 0; y < cy; y++)
	{
		for (int x = 0; x < cx; x++)
		{
			int tx = x;
			int ty = y;

			g_region->m_defScene->m_pTerrain->OffsetTerrainToWorld(tx, ty);

			const RtSceneBlockTerrain::STileAttr* attr = g_region->m_defScene->m_pTerrain->GetTerrainAttr(tx, ty);

			if (attr && attr->cAttr & mask)
			{
				buf[y * cx * 3 + x * 3 + 0] = 0x00;
				buf[y * cx * 3 + x * 3 + 1] = 0x00;
				buf[y * cx * 3 + x * 3 + 2] = 0x00;
			}
			if (attr && attr->pContainer && attr->pContainer->iBlock)
			{
				buf[y * cx * 3 + x * 3 + 0] = 0x3f;
				buf[y * cx * 3 + x * 3 + 1] = 0x3f;
				buf[y * cx * 3 + x * 3 + 2] = 0x3f;
			}
		}
	}

	SaveBitmap(filename, buf, cx, cy);

	DEL_ONE(buf);
}

void CRegionStateSnapshot::SaveBitmap(const char* filename, const unsigned char* buf, int width, int height)
{
	// BITMAPFILEHEADER can't use in linux
	// ...
	FILE* file = fopen(filename, "w+b");

	BITMAPFILEHEADER bmfh;
	bmfh.bfSize = sizeof(bmfh);
	bmfh.bfReserved1 = 0;
	bmfh.bfReserved2 = 0;
	bmfh.bfType = 'B' | ('M' << 8);
	bmfh.bfOffBits = sizeof(bmfh) + sizeof(BITMAPINFOHEADER);

	fwrite(&bmfh, 1, sizeof(bmfh), file);

	BITMAPINFOHEADER bmih;
	memset(&bmih, 0, sizeof(bmih));
	bmih.biSize = sizeof(bmih);
	bmih.biBitCount = 24;
	bmih.biWidth = width;
	bmih.biHeight = height;
	bmih.biPlanes = 1;

	fwrite(&bmih, 1, sizeof(bmih), file);

	fwrite(buf, 1, width * 3 * height, file);

	fclose(file);
}

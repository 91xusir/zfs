#include "core/rt2_core.h"
#include "resource.h"
#include "gc_cursor.h"

HCURSOR g_hCursors[GAME_CURSOR_MAX];

void GetCursorFileFromPackage(int id, std::string FileName)
{
	size_t nPos = FileName.rfind("\\");
	if (nPos == string::npos)
	{
		return;
	}

	char szPath[256];
	GetTempPath(256, szPath);

	std::string strName =szPath;

	strName += FileName.substr(nPos+1, FileName.size()-nPos-5);
	strName += ".~dat";

	RtArchive* readfile = RtCoreFile().CreateFileReader(FileName.c_str());
	if (!readfile) return;

	int nBufSize = readfile->TotalSize() + 1;
	char *pBuf = RT_NEW char[nBufSize];
	readfile->Serialize(pBuf, readfile->TotalSize());
	pBuf[nBufSize-1]='\0';
	readfile->Close();
	DEL_ONE(readfile);

	FILE *fp = fopen(strName.c_str(), "wb");
	if (!fp)
	{
		return;
	}

	fwrite(pBuf, 1, nBufSize, fp);
	fclose(fp);

	g_hCursors[id] = LoadCursorFromFile(strName.c_str());
}

void GameInitCursor()
{
#define LOAD_CURSOR(id, idc) \
	{ CHECK(id < GAME_CURSOR_MAX);  GetCursorFileFromPackage(id, idc);}

	//LOAD_CURSOR(GAME_CURSOR_NORMAL,         ".\\ui\\cur\\1.ani");
	LOAD_CURSOR(GAME_CURSOR_NORMAL,         ".\\ui\\cur\\normal.ani");
	LOAD_CURSOR(GAME_CURSOR_ATTACK,         ".\\ui\\cur\\2.ani");
	LOAD_CURSOR(GAME_CURSOR_GOODSKILL,      ".\\ui\\cur\\3.ani");
	LOAD_CURSOR(GAME_CURSOR_TALK,           ".\\ui\\cur\\4.ani");
	LOAD_CURSOR(GAME_CURSOR_DEITEM,         ".\\ui\\cur\\5.ani");
	LOAD_CURSOR(GAME_CURSOR_DIG,            ".\\ui\\cur\\6.ani");
	LOAD_CURSOR(GAME_CURSOR_PICK_HERB,      ".\\ui\\cur\\7.ani");
	//LOAD_CURSOR(GAME_CURSOR_CAN_NOT_MOVE,   ".\\ui\\cur\\8.ani");
	LOAD_CURSOR(GAME_CURSOR_CAN_NOT_MOVE,   ".\\ui\\cur\\cannotmove.ani");
	LOAD_CURSOR(GAME_CURSOR_PICK_ITEM,      ".\\ui\\cur\\9.ani");
	LOAD_CURSOR(GAME_CURSOR_GET_SKIN,       ".\\ui\\cur\\11.ani");
	LOAD_CURSOR(GAME_CURSOR_WALK_ALONE,     ".\\ui\\cur\\10.ani");
	LOAD_CURSOR(GAME_CURSOR_DRAG_MAP,       ".\\ui\\cur\\12.ani");
	LOAD_CURSOR(GAME_CURSOR_GATHER_TASK,    ".\\ui\\cur\\13.ani");
	LOAD_CURSOR(GAME_CURSOR_TRIGGER,        ".\\ui\\cur\\14.ani");
	LOAD_CURSOR(GAME_CURSOR_FIX,            ".\\ui\\cur\\15.ani");
	LOAD_CURSOR(GAME_CATCH_PET,             ".\\ui\\cur\\17.ani")

	GameSetCursor(GAME_CURSOR_NORMAL);
}

void GameClearCursor()
{
}

static GameCursor LastID=GAME_CURSOR_NORMAL;

void GameSetCursor(GameCursor id)
{
	if ((id < GAME_CURSOR_MAX) && (g_hCursors[id] != NULL))
	{
		LastID=id;
		SetCursor(g_hCursors[id]);
	}
}


void GameSetCursor()
{
	if ((LastID < GAME_CURSOR_MAX) && (g_hCursors[LastID] != NULL))
	{
		SetCursor(g_hCursors[LastID]);
	}
}


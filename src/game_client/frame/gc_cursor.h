#ifndef _GC_CURSOR_H
#define _GC_CURSOR_H


enum GameCursor
{
	GAME_CURSOR_NORMAL,             // 普通鼠标
	GAME_CURSOR_ATTACK,             // 战斗
	GAME_CURSOR_GOODSKILL,          // 有益技能
	GAME_CURSOR_TALK,               // 对话
	GAME_CURSOR_DEITEM,             // 分解
	GAME_CURSOR_DIG,                // 挖矿
	GAME_CURSOR_PICK_HERB,          // 采药
	GAME_CURSOR_CAN_NOT_MOVE,       // 不能移动
	GAME_CURSOR_PICK_ITEM,          // 拾取道具
	GAME_CURSOR_WALK_ALONE,         // 持续走路
	GAME_CURSOR_GET_SKIN,           // 剥皮
	GAME_CURSOR_DRAG_MAP,           // 拖动中地图
	GAME_CURSOR_GATHER_TASK,        //采集任务
	GAME_CURSOR_TRIGGER,            //机关触发
	GAME_CURSOR_FIX,				//修理
	GAME_CATCH_PET,                 //捕捉宠物
	GAME_CURSOR_SPLIT,				// 拆分装备
	GAME_CURSOR_MAX
};

extern void GameInitCursor();

extern void GameClearCursor();

extern void GameSetCursor(GameCursor cursor);
extern void GameSetCursor();


#endif // _GC_CURSOR_H

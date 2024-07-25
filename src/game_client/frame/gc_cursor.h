#ifndef _GC_CURSOR_H
#define _GC_CURSOR_H


enum GameCursor
{
	GAME_CURSOR_NORMAL,             // ��ͨ���
	GAME_CURSOR_ATTACK,             // ս��
	GAME_CURSOR_GOODSKILL,          // ���漼��
	GAME_CURSOR_TALK,               // �Ի�
	GAME_CURSOR_DEITEM,             // �ֽ�
	GAME_CURSOR_DIG,                // �ڿ�
	GAME_CURSOR_PICK_HERB,          // ��ҩ
	GAME_CURSOR_CAN_NOT_MOVE,       // �����ƶ�
	GAME_CURSOR_PICK_ITEM,          // ʰȡ����
	GAME_CURSOR_WALK_ALONE,         // ������·
	GAME_CURSOR_GET_SKIN,           // ��Ƥ
	GAME_CURSOR_DRAG_MAP,           // �϶��е�ͼ
	GAME_CURSOR_GATHER_TASK,        //�ɼ�����
	GAME_CURSOR_TRIGGER,            //���ش���
	GAME_CURSOR_FIX,				//����
	GAME_CATCH_PET,                 //��׽����
	GAME_CURSOR_SPLIT,				// ���װ��
	GAME_CURSOR_MAX
};

extern void GameInitCursor();

extern void GameClearCursor();

extern void GameSetCursor(GameCursor cursor);
extern void GameSetCursor();


#endif // _GC_CURSOR_H
